// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

/*
+======================================================================+
|Prototypes for extra needed function								   |
|These are not syscalls but needed to be used in exception.cc before   |
|executing syscall instructions                                        |
+======================================================================+
*/

// Start of constants =================================================
const int FILE_NAME_MAX_LEN = 32;
// End of constants ===================================================

/*
input: virtual address int @param: virtAddr
output: system data char* @return: data
purpose: convert virtual address to system data
note: the return data (char*) is allocated on heap, REMEMBER to delete it!!!
*/
char* User2System(int virtAddr, int limit);

/*
input: system data char* @param: buffer
input: virtual address int @param: virtAddr
input: length int @param: len
output: 0 if success, -1 if fail @return: result
purpose: convert system data to virtual address
note: buffer is allocated and managed by caller
*/
int System2User(int virtAddr, int len, char* buffer);

/*
+======================================================================+
|Start of exception hanlder								               |
+======================================================================+
*/

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Create:
			DEBUG(dbgSys, "[SC] SC_Create.\n");
			int virtAddr;
			char* filename;
			DEBUG(dbgSys, "\tReading virtual address of filename.\n");
			virtAddr = kernel->machine->ReadRegister(4);
			filename = User2System(virtAddr, FILE_NAME_MAX_LEN);

			//extra check
			if(!filename){
				DEBUG(dbgSys, "\tFatal: System memory drained\n");
				kernel->machine->WriteRegister(2, -1);
				return;
			}

			DEBUG(dbgSys, "\tFile name: " <<filename << "\n");
			int file_creation_result = SysCreate(filename);
			DEBUG(dbgSys, "\tFile creation result: " << file_creation_result << "\n");
			DEBUG(dbgSys, std::to_string(file_creation_result) << "\n");
			kernel->machine->WriteRegister(2, file_creation_result);


			delete[] filename;
			return;
			ASSERTNOTREACHED();
			break;
		break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

			break;

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}

/*
+======================================================================+
|"Extra needed functions" implementation                               |
+======================================================================+
*/

char* User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //needs for null terminating string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	//printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}
