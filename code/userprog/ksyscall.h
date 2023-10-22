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
#include "synchconsole.h"
#include "fdt.h"

static FileDescriptorTable fdt;

/**
 * Systemcall interface
 * These are the operations the Nachos kernel needs to support
 */
void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysCreate(char *filename)
{
  int res;

  if (kernel->fileSystem->Create(filename))
  {
    res = 0;
  }
  else
  {
    res = -1;
  }

  return res;
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

/**
 * Print a character on the console. If the console is not initialized yet,
 * do nothing.
 * @param c The character to print
 * @see ConsoleDriver::PutChar
 */
void SysPrintChar(char c)
{
  kernel->synchConsoleOut->PutChar(c);
  // kernel->GetConsole()->PutChar(c);
}

/**
 * Open a file with a specified OpenMode
 * @param filename The name of the file to open
 * @param mode (RO = 0, RW = 1)
 * RO: = 0 read only
 * RW: = 1 read and write
 * @return The file descriptor of the opened file, -1 if an error occured
 */
OpenFileID SysOpen(FileName filename, OpenMode mode)
{
  return fdt.Open(filename, mode);
}

/*
 * Close current open file in the file descriptor table
 * @param id The file descriptor of the file to close
 * @return 0 on success, -1 if an error occured
 */
int SysClose(OpenFileID id)
{
  return fdt.Close(id);
}

int consoleRead(char *buffer, int size)
{
  DEBUG(dbgFile, "\n\tReading from console with size " << size);
  int i = 0;
  char c;

  while (i < size)
  {
    
    c = kernel->synchConsoleIn->GetChar();
    DEBUG(dbgFile, "\n\tRead character " << c << " from console "<< i << " times");

    if (c == EOF)
    {
      DEBUG(dbgFile, "\n\tEnd of file reached");
      buffer[i] = '\0';
      break;
    }

    buffer[i] = c;
    ++i;
  }

  DEBUG(dbgFile, "\n\tValue: " << buffer);
  return i;
}

int SysRead(char *buffer, int size, OpenFileID fd)
{
  // check if fd is STDIN
  if (fd == STDIN)
  {
    return consoleRead(buffer, size);
  }

  int res;
  OpenFile *file = fdt.getFile(fd);

  // check for validity of file
  if (file == NULL)
  {
    res = -1;
  }
  else
  {
    res = file->Read(buffer, size);
  }

  DEBUG(dbgFile, "\n\tRead " << res << " characters from fd " << fd);
  DEBUG(dbgFile, "\n\tValue: " << buffer);
  return res;
}

int consoleWrite(char* buffer, int size){
  DEBUG(dbgFile, "\n\tWriting to console with size " << size);
  int i = 0;
  char c;

  while (i < size)
  {
    c = buffer[i];
    DEBUG(dbgFile, "\n\tWriting character " << c << " to console "<< i << " times");

    if (c == '\0')
    {
      DEBUG(dbgFile, "\n\tEnd of file reached");
      break;
    }

    kernel->synchConsoleOut->PutChar(c);
    ++i;
  }

  DEBUG(dbgFile, "\n\tValue: " << buffer);
  return i;
}

int SysWrite(char* buffer, int size, OpenFileID fd){
  // check if fd is STDOUT
  if (fd == STDOUT)
  {
    return consoleWrite(buffer, size);
  }

  DEBUG(dbgFile, "\n\tWriting to fd " << fd << " with size " << size);

  int res;
  OpenFile *file = fdt.getFile(fd);

  // check for validity of file
  if (file == NULL)
  {
    res = -1;
  }else if(fdt.isReadOnly(fd)){
    res = -1;
  }
  else
  {
    res = file->Write(buffer, size);
  }
  
  DEBUG(dbgFile, "\n\tWrote " << res << " characters to fd " << fd);
  DEBUG(dbgFile, "\n\tValue: " << buffer);
  return res;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
