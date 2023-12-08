#include "pcb.h"

void processCreator(void* arg)
{
 AddrSpace *space = (AddrSpace*)arg;
 space->Execute();
} 


PCB::PCB(int id)
{
    parentID = kernel->currentThread->getId(); /* create this function by yourself */
    pID = id;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    multex = new Semaphore("multex", 1);
}

PCB::~PCB()
{
    delete joinsem;
    delete exitsem;
    delete multex;
}

int PCB::Exec(char* tname,int pid)
{
    multex->P();

    OpenFile *executable = kernel->fileSystem->Open(tname);
    AddrSpace *space;
    space = new AddrSpace(executable);
    Thread * t = new Thread(tname);
    t->space = space;      // jump to the user progam

    t->Fork(&processCreator, (void*)space);
    
    multex->V();
}

int PCB::GetID()
{
    return pID;
}

int PCB::GetNumWait()
{
    return -1;
} 

void PCB::JoinWait()
{

}

void PCB::ExitWait()
{

}

void PCB::JoinRelease()
{

}

void PCB::ExitRelease()
{

}

void PCB::IncNumWait()
{

}

void PCB::DecNumWait()
{

}

void PCB::SetExitCode(int)
{

}

int PCB::GetExitCode()
{
    return -1;
}

void PCB::SetFileName(char*)    
{

}

char* PCB::GetFileName()
{
    return NULL;
}