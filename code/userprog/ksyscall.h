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

#include "hash.h"
#include "kernel.h"
#include "synchconsole.h"
#include "hash.h"

// Our own definitions
typedef int OpenMode;
typedef int OpenFileID;
typedef char *FileName;
// End of our own definitions

// Start of constants =================================================
const int MAX_INT = 2147483647;
const int STRING_MAX_LEN = MAX_INT;
const int FILE_NAME_MAX_LEN = 32;
const int MAX_FILE_DESCRIPTOR = 20;
const OpenMode RO = 0;
const OpenMode RW = 1;
// file descriptor table

static OpenFile *fdt[MAX_FILE_DESCRIPTOR]{NULL};
// End of constants ====================================================
/**
 * @brief      Gets the free file descriptors.
 * @return     The free file descriptor on success, -1 on fail (is full).
 */
OpenFileID getFreeFileDescriptor()
{
  OpenFileID fd;
  fd = -1;
  // we start from 2 because 0 and 1 are reserved for stdin and stdout
  for (int i = 2; i < MAX_FILE_DESCRIPTOR; i++)
  {
    if (!fdt[i])
    {
      fd = i;
      break;
    }
  }
  return fd;
}

/**
 * @brief      Counts the file descriptors.
 * @return     The number of file descriptor (>=2) reserved for stdin, stdout.
 */
int countFileDescriptor()
{
  int count;
  count = 0;
  for (int i = 2; i < MAX_FILE_DESCRIPTOR; i++)
  {
    if (fdt[i])
    {
      count++;
    }
  }
  return count;
}

/**
 * @brief      Gets the file.
 * @param[in]  fd    The file descriptor
 * @return     The file on success, NULL on fail.
 */
OpenFile *getFile(OpenFileID fd)
{
  if (fd < 0 || fd >= MAX_FILE_DESCRIPTOR || fd == 0 || fd == 1)
  {
    DEBUG(dbgFile, "\n\tInvalid file descriptor " << fd);
    return NULL;
  }
  OpenFile *file = fdt[fd];
  return file;
}

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
  DEBUG(dbgFile, "\n\tOpening file " << filename << " with mode " << mode);
  OpenFile *file = kernel->fileSystem->Open(filename);

  if (!file)
  {
    DEBUG(dbgFile, "\n\tFile not found or file currently cannot be opened " << filename);
    return -1;
  }
  else
  {
    OpenFileID id = getFreeFileDescriptor();

    if (id == -1)
    {
      DEBUG(dbgFile, "\n\tNo free file descriptors");
      return -1;
    }
    else
    {
      DEBUG(dbgFile, "\n\tFile opened with id " << id);
      fdt[id] = file;
      return id;
    }
  }
}

/*
 * Close current open file in the file descriptor table
 * @param id The file descriptor of the file to close
 * @return 0 on success, -1 if an error occured
 */
int SysClose(OpenFileID id)
{
  DEBUG(dbgFile, "\n\tClosing file descriptor " << id)
  if (id < 2 || id >= MAX_FILE_DESCRIPTOR)
  {
    DEBUG(dbgFile, "\n\tInvalid file descriptor " << id);
    return -1;
  }
  else
  {
    OpenFile *file;
    file = getFile(id);
    if (file == NULL)
    {
      DEBUG(dbgFile, "\n\tFile descriptor " << id << " is not open");
      return -1;
    }
    else
    {
      file->~OpenFile();
      DEBUG(dbgFile, "\n\tFile descriptor " << id << " closed");
      fdt[id] = NULL;
      return 0;
    }
  }
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
