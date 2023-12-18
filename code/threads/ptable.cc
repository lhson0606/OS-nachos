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
    //mark the slot is used
    bm->Mark(freeSlot);

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

int PTable::ExitUpdate(int exitCode)
{
    int pid = kernel->currentThread->getId();

    if(pid == -1)
    {
        //main process
        return -1;
    }

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
        curPcb->ExitWait();
    }
   
    curPcb->Exit();
    //remove the process from the table
    Remove(pid);
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
    //iterate through the table, return the first free slot
    for(int i = 0; i < psize; i++)
    {
        if(!bm->Test(i))
        {
            bmsem->V();
            return i;
        }
    }
    bmsem->V();
    //if no free slot is found, return -1
    return -1;
}

bool PTable::IsExist(int pid)
{
    //we need mutex here, multiple process can call this function at the same time
    DEBUG(dbgThread, "\n\tReached 1\n");
    bmsem->P();
    DEBUG(dbgThread, "\n\tReached 2\n");
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