#include "pcb.h"

PCB::PCB(int id)
{

}

PCB::~PCB()
{

}

int PCB::Exec(char* name,int pid)
{
    return -1;
}

int PCB::GetID()
{
    return -1;
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