#ifndef _STABLE_H_
#define _STABLE_H_

#include "sem.h"
#include "bitmap.h"

#define MAX_PROCESS 10
#define MAX_SEMAPHORE 10

class STable
{ 
public:// Initial the Sem object, the started value is null
    // Remember to initial the bm object to use
    STable();

    ~STable();
    // Check the semaphore name, create a new one if not already
    int Create(char *name, int init);
    // If the semaphore name already exists, call this->P() to execute it.
    // If not, report an error in Wait, Signal function
    int Wait(char *name);
    // If the semaphore name already exists, call this->V() to execute it.
    // If not, report an error in Wait, Signal function
    int Signal(char *name);

    int FindFreeSlot(); // Find an empty slot

private:
    Bitmap* bm; // Manage the free slot
    Sem* semTab[MAX_SEMAPHORE];
};

#endif // _STABLE_H_