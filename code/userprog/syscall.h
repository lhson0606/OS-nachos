/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"
#include "errno.h"
/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt		        0
#define SC_Exit		        1
#define SC_Exec		        2
#define SC_Join		        3
#define SC_Create	        4
#define SC_Remove           5
#define SC_Open		        6
#define SC_Read		        7
#define SC_Write	        8
#define SC_Seek             9
#define SC_Close            10
#define SC_ThreadFork	    11
#define SC_ThreadYield	    12
#define SC_ExecV	        13
#define SC_ThreadExit       14
#define SC_ThreadJoin       15

//command line arguments
#define SC_GetArgvs         20

//Arithmetic operations
#define SC_Add		        42

//Console operations    
#define SC_PrintStrn	    50

//Socket operations 
#define SC_SocketTCP	    60
#define SC_Connect		    61
#define SC_Send			    62
#define SC_Receive		    63
#define SC_Disconnect	    64
#define SC_ServerCreate     65
#define SC_ServerListen     66

//semaphore
#define SC_CreateSem        69
#define SC_WaitSem          70
#define SC_SignalSem        71

#ifndef IN_ASM

/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the 
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking, 
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt();		
 
 
/*
 * Add the two operants and return the result
 */ 

int Add(int op1, int op2);

/* Address space control operations: Exit, Exec, Execv, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit(int status);	

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;	

/* A unique identifier for a thread within a task */
typedef int ThreadId;

/* Run the specified executable, with no args */
/* This can be implemented as a call to ExecV.
 */ 
SpaceId Exec(char* exec_name);

/* Run the executable, stored in the Nachos file "argv[0]", with
 * parameters stored in argv[1..argc-1] and return the 
 * address space identifier
 */
SpaceId ExecV(int argc, char* argv[]);
 
/* Only return once the user program "id" has finished.  
 * Return the exit status.
 */
int Join(SpaceId id); 	
 

/* File system operations: Create, Remove, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * Note that the Nachos file system has a stub implementation, which
 * can be used to support these system calls if the regular Nachos
 * file system has not been implemented.
 */
 
/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;	

/* when an address space starts up, it has two open files, representing 
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

//#define ConsoleInput 0
//#define ConsoleOutput 1 
 
/* Create a Nachos file, with name "name" */
/* Note: Create does not open the file.   */
/* Return 1 on success, negative error code on failure */
int Create(char *name);

/*
 * Prints a string to the console.
 */
int PrintStrn(char* strn);

/* Remove a Nachos file, with name "name" */
int Remove(char *name);

/* Open the Nachos file "name", and return an "OpenFileId" that can 
 * be used to read and write to the file.
 */
OpenFileId Open(char *name, int type);

/* Write "size" bytes from "buffer" to the open file. 
 * Return the number of bytes actually read on success.
 * On failure, a negative error code is returned.
 */
int Write(char *buffer, int size, OpenFileId id);

/* Read "size" bytes from the open file into "buffer".  
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
int Read(char *buffer, int size, OpenFileId id);

/* Set the seek position of the open file "id"
 * to the byte "position".
 */
int Seek(int position, OpenFileId id);

/* Close the file, we're done reading and writing to it.
 * Return 1 on success, negative error code on failure
 */
int Close(OpenFileId id);

/**
 * Creates a new socket and returns the socket descriptor.
 * @return The socket descriptor on success, -1 on failure.
*/
int SocketTCP();
/**
 * Connects to a socket.
 * @param socketid The socket descriptor.
 * @param ip The ip address to connect to.
 * @param port The port to connect to.
 * @return 0 on success, -1 on failure.
*/
int Connect(int socketid, char *ip, int port);

/**
 * Sends a message to a socket.
 * @param socketid The socket descriptor.
 * @param buffer The message to send.
 * @param len The length of the message.
 * @return The number of bytes sent on success, -1 on failure.
*/
int Send(int socketid, char *buffer, int len);

/**
 * Receives a message from a socket.
 * @param socketid The socket descriptor.
 * @param buffer The buffer to store the message in.
 * @param len The length of the buffer.
 * @return The number of bytes received on success, -1 on failure.
*/
int Receive(int socketid, char *buffer, int len);

/**
 * Disconnects from a socket.
 * @param socketid The socket descriptor.
 * @return 0 on success, -1 on failure.
*/
int Disconnect(int socketid);

/**
 * Creates a new server socket and returns the socket descriptor.
 * @param port The port to listen on.
 * @return The socket descriptor on success, -1 on failure.
*/
int ServerCreate(int port);

/**
 * Listens on a server socket.
 * @param ss_fd The socket descriptor.
 * @return 0 on success, -1 on failure.
*/
int ServerListen(int ss_fd);

/**
 * Returns the command line arguments of the current process.
 * @param argc The number of arguments that user program wants to get.
 * @param argv The arguments buffer.
 * @param size The size of an argument buffer.
 * @return number of arguments on success, -1 on failure.
*/
int GetArgvs(int argc, char* argv[], int size);

/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 *
 * Could define other operations, such as LockAcquire, LockRelease, etc.
 */

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 * Return a positive ThreadId on success, negative error code on failure
 */
ThreadId ThreadFork(void (*func)());

/* Yield the CPU to another runnable thread, whether in this address space 
 * or not. 
 */
void ThreadYield();	

/*
 * Blocks current thread until lokal thread ThreadID exits with ThreadExit.
 * Function returns the ExitCode of ThreadExit() of the exiting thread.
 */
int ThreadJoin(ThreadId id);

/*
 * Deletes current thread and returns ExitCode to every waiting lokal thread.
 */
void ThreadExit(int ExitCode);

int CreateSemaphore(char* name, int semval);

int Wait(char* name);

int Signal(char* name);

#endif /* IN_ASM */

#endif /* SYSCALL_H */

