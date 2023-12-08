//
// Created by khuong on 12/7/23.
//

#include "bitmap.h"
#include "semaphore.h
#define MAX_SEMAPHORE 10

class STable {
private:
    Bitmap* bm;
    Semaphore* semTable[MAX_SEMAPHORE];

public:
    STable(){};
    ~STable(){};
    int Create(char* name,int init);
    int Wait(char* name);
    int Signal(char* name);
    int FindFreeSlot(int id);
};