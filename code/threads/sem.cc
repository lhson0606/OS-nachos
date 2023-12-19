#include "sem.h"

Sem::Sem(char* sname, int permits)
{
    strcpy(this->name, sname);
    sem = new Semaphore(this->name, permits);
}

Sem::~Sem() // Destruct the Sem object
{
    if(sem)
    delete sem;
} 

void Sem::wait()
{
    sem->P(); // Conduct the waiting function
} 

void Sem::signal()
{
    sem->V(); // Release semaphore
}

char* Sem::GetName() // Return the semaphore name
{
    return this->name;
}