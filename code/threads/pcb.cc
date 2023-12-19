#include "pcb.h"
#include "debug.h"

void processCreator(void* arg)
{
 AddrSpace *space = (AddrSpace*)arg;
 if(space->Load(space->executable)){
    DEBUG(dbgThread, "New process created "<< kernel->currentThread->getName() << " with pid " << kernel->currentThread->getId() << "\n");
    space->Execute();
 }

 ASSERTNOTREACHED();
} 


PCB::PCB(int id)
{
    if(id == 0)//our main thread
        parentID = -1;
    else{
        parentID = kernel->currentThread->getId();
    }
    
    pID = id;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    multex = new Semaphore("multex", 1);
    shouldExit = new Semaphore("shouldExit", 1);
}

PCB::~PCB()
{
    delete joinsem;
    delete exitsem;
    delete multex;
    delete shouldExit;
    delete[] filename;
    
}

int PCB::Exec(char* tname,int pid)
{
    int length = strlen(tname);
    filename = new char[length+1];
    strcpy(filename, tname);
    filename[length] = '\0';
    DEBUG(dbgThread, "\n\t"<<kernel->currentThread->getName() << " executing " << filename << " with pid " << pid << "\n");
    multex->P();

    OpenFile *executable = kernel->fileSystem->Open(tname);
    //#todo check if executable is null
    ASSERT(executable != NULL);
    AddrSpace *space;
    space = new AddrSpace(executable);
    //#todo implement an actual process data structure, with a kernel thread which is the executor
    Thread * t = new Thread(tname, pid);
    t->space = space;      // jump to the user progam

    t->Fork(&processCreator, (void*)space);
    
    multex->V();
    return pid;
}

void PCB::SetArgvs(int argc, char* argv[]){
    userArgc = argc;
    for(int i = 0; i < argc; i++){
        userArgs[i] = argv[i];
    }
}

int PCB::GetArgc()
{
    return userArgc;
}

char** PCB::GetArgv()
{
    return userArgs;
}

int PCB::GetID()
{
    return pID;
}

int PCB::GetNumWait()
{
    return numwait;
} 

void PCB::JoinWait()
{
    //wait for the child process to exit
    joinsem->P();
}

void PCB::ExitWait()
{
    //allow the child process to exit
    exitsem->P();
    //releae the exitSem so that the other child process can exit
    exitsem->V();
}

void PCB::JoinRelease()
{
    //release the join semaphore
    joinsem->V();
}

void PCB::ExitRelease()
{
    //release the exit semaphore, this will be called by the parent process
    exitsem->V();
}

void PCB::IncNumWait()
{
    multex->P();
    DEBUG(dbgThread, kernel->currentThread->getName() << " increasing " << numwait << " of process " << pID << " to " << numwait + 1 << "\n");
    //if we are the first process to wait, we need to obtain the shouldExit semaphore
    if(numwait == 0)
    {
        shouldExit->P();
    }

    numwait++;
    multex->V();
}

void PCB::DecNumWait()
{
    multex->P();
    numwait--;
    //if we are the last child process to exit, we need to release the shouldExit semaphore
    if(numwait == 0)
    {
        shouldExit->V();
    }

    multex->V();
}

void PCB::SetExitCode(int exitCode)
{
    this->exitcode = exitCode;
}

int PCB::GetExitCode()
{
    return exitcode;
}

void PCB::SetFileName(char*)    
{
    //thinking about this (allocation and deallocation name) since we are using char* instead of string
    //what will happen if we accidentally delete the name of the process that has not been allocated yet?
}

char* PCB::GetFileName()
{
    //the caller should not delete the name of the process
    return filename;
}

int PCB::GetParentID()
{
    return parentID;
}

void PCB::Exit(){
    shouldExit->P();
    DEBUG(dbgThread, GetFileName() << " exiting with exitcode " << exitcode << "\n");
    //#todo: implement this(release resources include memory, files, etc. hoding by the process)
}