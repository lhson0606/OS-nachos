/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"




void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysCreate(char* filename)
{
  int res;

  if(kernel->fileSystem->Create(filename)){
    res = 0;
  } else{
    res = -1;
  }

  return res;
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}




#endif /* ! __USERPROG_KSYSCALL_H__ */
