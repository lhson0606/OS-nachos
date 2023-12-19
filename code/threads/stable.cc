#include "stable.h"

STable::STable()
{
    bm = new Bitmap(MAX_SEMAPHORE);
}

STable::~STable()
{
    delete bm;
    
    for(int i = 0; i < MAX_SEMAPHORE; i++){
        delete semTab[i];
    }
}

int STable::Create(char *name, int init)
{
    int id = FindFreeSlot();

    if(id == -1){
        return -1;
    }
    semTab[id] = new Sem(name, init);
}

int STable::Wait(char *name)
{
    for(int i = 0; i < MAX_SEMAPHORE; i++){
        if(semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0){
            semTab[i]->wait();
            return i;
        }
    }

    return -1;
}

int STable::Signal(char *name)
{
    for(int i = 0; i < MAX_SEMAPHORE; i++){
        if(semTab[i] != NULL && strcmp(semTab[i]->GetName(), name) == 0){
            semTab[i]->signal();
            return i;
        }
    }

    return -1;
}

int STable::FindFreeSlot()
{
    return bm->FindAndSet();
}