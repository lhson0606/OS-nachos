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
    bmsem->P();

    int freeSlot = GetFreeSlot();

    if(freeSlot == -1)
    {
        bmsem->V();
        return -1;
    }
    //note that we our table is called "pcb" already
    PCB* aNewPcb = new PCB(freeSlot);
    //add our new pcb to the table
    pcb[freeSlot] = aNewPcb;
    //mark the slot is used
    bm->Mark(freeSlot);
    bmsem->V();

    return aNewPcb->Exec(filename, freeSlot);
}

int PTable::ExitUpdate(int)
{
    return -1;
}

int PTable::JoinUpdate(int)
{
    return -1;
}

int PTable::GetFreeSlot()
{
    //iterate through the table, return the first free slot
    for(int i = 0; i < psize; i++)
    {
        if(!bm->Test(i))
        {
            return i;
        }
    }
    //if no free slot is found, return -1
    return -1;
}

bool PTable::IsExist(int pid)
{
    //iterate through the table, return true if the pid is found
    for(int i = 0; i < psize; i++)
    {
        if(pcb[i]->GetID() == pid)
        {
            return true;
        }
    }
    //if no pid is found, return false
    return false;
}

void PTable::Remove(int pid)
{

}

char* PTable::GetFileName(int id)
{
    return NULL;
}