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

/**
 * @brief      Converts a virtual address to system address.
 * @param[in]  virtAddr  The virtual address
 * @param[in]  limit     The limit
 * @return     The system address.
 * @note       The caller have responsibility to free the returned buffer.
*/
char* User2System(int virtAddr, int limit);

/**
 * @brief      Converts a system address to virtual address.
 * @param[in]  virtAddr  The virtual address
 * @param[in]  len       The length
 * @param      buffer    The buffer
 * @return     The virtual address.
 * @note       The caller have responsibility to free the returned buffer.
*/
int System2User(int virtAddr, int len, char* buffer);

/**
 * @brief      Increaments the PC.
*/
void increasePC();

/*
+======================================================================+
|Start of exception hanlder								               |
+======================================================================+
*/

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);
	int virtAddr;

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
			case SC_Halt:
			{
				DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

				SysHalt();

				ASSERTNOTREACHED();
				break;	
			}				

			case SC_Create:
			{
				DEBUG(dbgSys, "[SC] SC_Create.\n");
				char* filename;
				int file_creation_result;
				file_creation_result = -1;

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
				file_creation_result = SysCreate(filename);
				DEBUG(dbgSys, "\tFile creation result: " << file_creation_result << "\n");
				kernel->machine->WriteRegister(2, file_creation_result);

				delete[] filename;
				increasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}
				

			case SC_Open:
			{
				DEBUG(dbgSys, "[SC] SC_Open.\n");
				char* filename;
				int open_result;
				OpenMode mode;
				OpenFileID fileID;
				open_result = -1;

				DEBUG(dbgSys, "\tReading virtual address of filename.\n");
				virtAddr = kernel->machine->ReadRegister(4);
				mode = (OpenMode)kernel->machine->ReadRegister(5);
				filename = User2System(virtAddr, FILE_NAME_MAX_LEN);

				//extra check
				if(!filename){
					DEBUG(dbgSys, "\tFatal: System memory drained\n");
					kernel->machine->WriteRegister(2, -1);
					return;
				}

				//check for mode validity
				if(mode != RO && mode != RW){
					DEBUG(dbgSys, "\tFatal: Invalid mode\n");
					kernel->machine->WriteRegister(2, -1);
					return;
				}

				DEBUG(dbgSys, "\tFile name: " <<filename << "\n");
				fileID = SysOpen(filename, mode);
				open_result = (int)fileID;//extra steps for modifying in the future
				
				if(open_result!=-1){
					DEBUG(dbgSys, "\tFile opened successfully with ID: " << fileID << "\n");
				}else{
					DEBUG(dbgSys, "\tFile opened failed name: " << filename << "\n");
				}				
				
				kernel->machine->WriteRegister(2, open_result);

				delete[] filename;
				increasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}

			case SC_Close:
			{
				DEBUG(dbgSys, "[SC] SC_Close.\n");
				int close_result;
				OpenFileID fileID;
				close_result = -1;

				fileID = kernel->machine->ReadRegister(4);
				DEBUG(dbgSys, "\tFile input ID: "<<fileID<<"\n");

				close_result = SysClose(fileID);
				
				if(close_result!=-1){
					DEBUG(dbgSys, "\tFile ID closed successfully - ID: " << close_result << "\n");
				}else{
					DEBUG(dbgSys, "\tFile closed failed - ID: " << fileID << "\n");
				}				
				
				kernel->machine->WriteRegister(2, close_result);

				increasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}	

			case SC_Add:
			{
				DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

				/* Process SysAdd Systemcall*/
				int result;
				result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
								/* int op2 */ (int)kernel->machine->ReadRegister(5));

				DEBUG(dbgSys, "Add returning with " << result << "\n");
				/* Prepare Result */
				kernel->machine->WriteRegister(2, (int)result);

				increasePC();

				return;

				ASSERTNOTREACHED();
				break;
			}

			case SC_PrintStrn:
			{
				DEBUG(dbgSys, "[SC] SC_PrintStrn.\n");
				char* strn;

				virtAddr = kernel->machine->ReadRegister(4);
				strn = User2System(virtAddr, STRING_MAX_LEN);

				//extra check
				if(!strn){
					DEBUG(dbgSys, "\tFatal: System memory drained\n");
					kernel->machine->WriteRegister(2, -1);
					return;
				}

				int count;
				count = 0;

				while(strn[count] != '\0' && count<STRING_MAX_LEN){
					SysPrintChar(strn[count]);
					count++;
				}
				
				DEBUG(dbgSys, "\tString: " << strn << "\n");
				int strn_len;
				strn_len = strnlen(strn, STRING_MAX_LEN);
				DEBUG(dbgSys, "\tString length: " << strn_len << "\n");
				DEBUG(dbgSys, "\tPrinted: " << count << " character(s)\n");
				kernel->machine->WriteRegister(2, count);
				delete[] strn;
				strn = NULL;
				
				increasePC();
				return;
				ASSERTNOTREACHED();
				break;
			}		
			
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
	kernelBuf[limit] = '\0';//just to make sure
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

void increasePC(){
	/* Modify return point */
	{
		/* set previous programm counter (debugging only)*/
		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

		/* set next programm counter for brach execution */
		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
	}
}