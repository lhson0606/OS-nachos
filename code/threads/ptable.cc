#include "ptable.h"

PTable::PTable(int size)
{
    psize = size;
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem", 1);
}

PTable::~PTable()
{
    delete bm;
    delete bmsem;
}

int PTable::ExecUpdate(char* filename)
{
    int freeSlot = GetFreeSlot();
    //call after we get the free slot because we will obtain lock in GetFreeSlot()
    bmsem->P();

    if(freeSlot == -1)
    {
        bmsem->V();
        return -1;
    }
    //note that our table is called "pcb" already
    PCB* aNewPcb = new PCB(freeSlot);
    //add our new pcb to the table
    pcb[freeSlot] = aNewPcb;

    bmsem->V();
    //we will release the lock before calling IncNumWait() in parent process
    int parentID = kernel->currentThread->getId();
    if(parentID != -1)
    {
        PCB* parent = pcb[parentID];
        parent->IncNumWait();
    }

    aNewPcb->Exec(filename, freeSlot);
    return freeSlot;
}

int PTable::ExecVUpdate(int argc, char* argv[]){
    int freeSlot = GetFreeSlot();
    //call after we get the free slot because we will obtain lock in GetFreeSlot()
    bmsem->P();

    if(freeSlot == -1)
    {
        bmsem->V();
        return -1;
    }
    //note that our table is called "pcb" already
    PCB* aNewPcb = new PCB(freeSlot);
    aNewPcb->SetArgvs(argc, argv);
    //add our new pcb to the table
    pcb[freeSlot] = aNewPcb;

    bmsem->V();
    //we will release the lock before calling IncNumWait() in parent process
    int parentID = kernel->currentThread->getId();
    if(parentID != -1)
    {
        PCB* parent = pcb[parentID];
        parent->IncNumWait();
    }

    aNewPcb->Exec(argv[0], freeSlot);
    return freeSlot;
}

PCB* PTable::GetPcb(int id)
{
    if(id == -1 || id > psize-1) return NULL;
    return pcb[id];
}

int PTable::ExitUpdate(int exitCode)
{
    int pid = kernel->currentThread->getId();
    DEBUG(dbgThread, kernel->currentThread->getName()<< " pId "<< pid << " exiting with code " << exitCode << "\n");
    
    ASSERT(IsExist(pid));//#todo: implement this instead of assert
   
    PCB* curPcb = pcb[pid];
    ASSERT(curPcb != NULL);
    int parentID = curPcb->GetParentID();

    curPcb->SetExitCode(exitCode);
    curPcb->JoinRelease();
    curPcb->ExitRelease();

    if(parentID != -1)
    {
        PCB* parent = pcb[parentID];
        parent->DecNumWait();
        //wait for the parent to accept the exit
        parent->ExitWait();
    }

    if(pid ==0){
        kernel->interrupt->Halt();
    }else{
        curPcb->Exit();    
        //remove the process from the table
        Remove(pid);
        kernel->currentThread->Finish();
    }
    
    return exitCode;
}

int PTable::JoinUpdate(int pID)
{
    //should only call by the parent process so we don't need to obtain the lock
    if(!IsExist(pID))
    {
        ASSERTNOTREACHED();
        return -1;
    }

    PCB* childPcb = pcb[pID];
    ASSERT(childPcb != NULL);//we should alter this to something instead of assert
    DEBUG(dbgThread, kernel->currentThread->getName() << " waiting " << childPcb->GetFileName() << " to finish\n");
    childPcb->JoinWait();
    int exitCode = childPcb->GetExitCode();
    return exitCode;
}

int PTable::GetFreeSlot()
{
    //we need mutex here, multiple process can call this function at the same time
    bmsem->P();
    int result = bm->FindAndSet();
    bmsem->V();
    return result;
}

bool PTable::IsExist(int pid)
{
    //we need mutex here, multiple process can call this function at the same time
    bmsem->P();
    if(pid <0 || pid >= psize)
    {
        bmsem->V();
        return false;
    }
    PCB* curPcb = pcb[pid];
    //if no pid is found, return false
    if(curPcb == NULL)
    {
        bmsem->V();
        return false;
    }

    bmsem->V();
    return true;
}

void PTable::Remove(int pid)
{
    bmsem->P();
    //remove the process from the table
    DEBUG(dbgThread, "PTable removing pid " << pid << "\n");
    bm->Clear(pid);
    PCB* curPcb = pcb[pid];
    ASSERT(curPcb != NULL);
    delete curPcb;
    pcb[pid] = NULL;
    bmsem->V();
}

char* PTable::GetFileName(int id)
{
    if(id == -1) return "main";
    return pcb[id]->GetFileName();
}

void PTable::StartMainThread(char* filename){
    DEBUG(dbgThread, "Initializing user program: "<<filename);
    OpenFile *executable = kernel->fileSystem->Open(filename);
    AddrSpace *space = new AddrSpace(executable);
    PCB* mainPcb = new PCB(0);
    pcb[0] = mainPcb;
    char** userArgs = new char*[kernel->userArgc+1];
    int length = strlen(filename);
    userArgs[0] = new char[length+1];
    strcpy(userArgs[0], filename);
    userArgs[0][length] = '\0';
    kernel->currentThread->setId(0);

    for(int i = 1; i < kernel->userArgc+1; i++){
        int length = strlen(kernel->userArgs[i-1]);
        userArgs[i] = new char[length+1];
        strcpy(userArgs[i], kernel->userArgs[i-1]);
        userArgs[i][length] = '\0';
    }

    mainPcb->SetArgvs(kernel->userArgc+1, userArgs);
    bm->Mark(0);

    ASSERT(space != (AddrSpace *)NULL);
    if (space->Load(executable)) {  // load the program into the space
        space->Execute();              // run the program
    }
}