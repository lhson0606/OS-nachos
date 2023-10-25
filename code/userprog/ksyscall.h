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
#include "openfile.h"

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
 * Print a string on the console
 * @param string The null-terminated string to print
 * @see ConsoleDriver::PutString
 */
void SysPrintString(char *string)
{
  while(*string != '\0'){
    kernel->synchConsoleOut->PutChar(*string);
    string++;
  }
  // kernel->GetConsole()->PutString(string);
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

    if (c == EOF || c == '\0' || c == '\n')
    {
      DEBUG(dbgFile, "\n\tEnd string reached");
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

void exitWithError(char* msg){
  SysPrintString(msg);
  kernel->interrupt->Halt();
}


/**
 * Seek to a specified position in a file
 * @param pos The position to seek to
 * @param fd The file descriptor of the file to seek in
 * @return The new position in the file, -1 if an error occured
*/
int SysSeek(int pos, OpenFileID fd){
  int res = pos;
  OpenFile *file = fdt.getFile(fd);

  // check for validity of file
  if(fd == STDIN || fd == STDOUT){
    exitWithError("Cannot seek on STDIN or STDOUT");
  }
  else if (file == NULL)
  {
    return -1;
  }
  else if(res == -1){//seek to EOF
    pos = file->Length();
  }

  file->Seek(pos);
  res = pos;

  DEBUG(dbgFile, "\n\tSeeked to position " << pos << " in fd " << fd);
  return res;
}

/**
 * Remove a file give a given name
 * @param filename The name of the file to remove
 * @return 0 on success, -1 if an error occured
 */
int SysRemove(char* filename){
  int res;

  //check if file is opening or not
  if(fdt.isOpen(filename)){
    DEBUG(dbgFile, "\n\tCannot remove file " << filename << " because it is opening");
    return -1;
  }

  if (kernel->fileSystem->Remove(filename))
  {
    res = 0;
  }
  else
  {
    res = -1;
  }

  DEBUG(dbgFile, "\n\tRemoved file " << filename);
  return res;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
