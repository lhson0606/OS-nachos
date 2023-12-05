#ifndef _SEM_H_
#define _SEM_H_

#include "synch.h"

class Sem
{ 
public:
    // Initial the Sem object, the started value is null
    // Remember to initial the Sem to use
    Sem(char* na, int i);

    ~Sem();

    void wait();

    void signal();

    char* GetName();

private:
    char name[50]; // The semaphore nameSemaphore* sem; // Create semaphore for management
    Semaphore* sem;
};

#endif // _SEM_H_