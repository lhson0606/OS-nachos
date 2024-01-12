#ifndef _SEM_H_
#define _SEM_H_

#include "synch.h"

#define SEM_NAME_MAX_LEN 64

class Sem
{ 
public:
    // Initial the Sem object, the started value is null
    // Remember to initial the Sem to use
    Sem(char* sname, int permits);

    ~Sem();

    void wait();

    void signal();

    char* GetName();

private:
    char name[SEM_NAME_MAX_LEN]; // The semaphore nameSemaphore* sem; // Create semaphore for management
    Semaphore* sem;
};

#endif // _SEM_H_