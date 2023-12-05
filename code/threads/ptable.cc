#include "ptable.h"

PTable::PTable(int size)
{
    size = -1;
}

PTable::~PTable()
{
}

int PTable::ExecUpdate(char*)
{
    return -1;
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
    return -1;
}

bool PTable::IsExist(int pid)
{
    return false;
}

void PTable::Remove(int pid)
{

}

char* PTable::GetFileName(int id)
{
    return NULL;
}