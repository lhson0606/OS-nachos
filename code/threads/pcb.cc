#include "pcb.h"
#include "debug.h"

void processCreator(void* arg)
{
    DEBUG(dbgThread, "Entering processCreator");
    //I have no idea what this does :)
 AddrSpace *space = (AddrSpace*)arg;
 if(space->Load(space->executable)){
    space->Execute();
 }
 ASSERTNOTREACHED();
} 


PCB::PCB(int id)
{
    parentID = kernel->currentThread->getId(); /* create this function by yourself */
    pID = id;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    multex = new Semaphore("multex", 1);
}

PCB::~PCB()
{
    delete joinsem;
    delete exitsem;
    delete multex;
}

int PCB::Exec(char* tname,int pid)
{
    DEBUG(dbgThread, "Entering Exec");
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
    DEBUG(dbgThread, "New thread created");
    DEBUG(dbgThread, "Entering processCreator");
    //I have no idea what this does :)
    // if(space->Load(space->executable)){
    //     multex->V();
    //     space->Execute();
    // }
    
    multex->V();
}

int PCB::GetID()
{
    return pID;
}

int PCB::GetNumWait()
{
    return -1;
} 

void PCB::JoinWait()
{

}

void PCB::ExitWait()
{

}

void PCB::JoinRelease()
{

}

void PCB::ExitRelease()
{

}

void PCB::IncNumWait()
{

}

void PCB::DecNumWait()
{

}

void PCB::SetExitCode(int)
{

}

int PCB::GetExitCode()
{
    return -1;
}

void PCB::SetFileName(char*)    
{

}

char* PCB::GetFileName()
{
    return NULL;
}