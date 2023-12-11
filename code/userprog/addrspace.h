// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "openfile.h"

class Thread;

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    //#todo: remember to free this, oh boy I'm so depressed :)
    OpenFile *executable;		// The file containing the code to run

    AddrSpace();			// Create an address space.
    AddrSpace(OpenFile *executable);	// Create an address space,
          // initializing it with the program
          // stored in the file "executable"
    ~AddrSpace();			// De-allocate an address space
    //This is our custom load version, loads the code into memory for multiprogramming version
    bool Load(OpenFile* executable);

    bool Load(char *fileName);		// Load a program into addr space from
                                        // a file
					// return false if not found

    void Execute();             	// Run a program
					// assumes the program has already
                                        // been loaded

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 

    // Translate virtual address _vaddr_
    // to physical address _paddr_. _mode_
    // is 0 for Read, 1 for Write.
    ExceptionType Translate(unsigned int vaddr, unsigned int *paddr, int mode);

    void setInvalidAccessingAddr(int addr);

    int getAndResetInvalidAccessingAddr();

    void saveToBackingStore(int vpn);

    void loadFromBackingStore(int vpn);

    bool isDirty(int vpn);

    void setValidPage(int vpn, bool val);

    void updatePageTable(int vpn, int ppn);

    void initBackingStore(int size);

    int getVirtualPage(int ppn);
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code
    /**
     * use this when the memory is not contiguous in multiprogramming version
     * assume that constructor has already been called
    */
    void LoadIntoMemory(OpenFile *executable, int startAddr, int size, int virtualAddr);
    int invalidAccessingAddr = -1; //used to store the address that caused the exception
    OpenFile* backingStore = NULL;
    static int backingStoreID;
};

// This class is used to store which physical page is allocated to which space
// so that we can we can perform swapping when needed
// probably need a separate file for this, but I'm too lazy to do that :)
class AddrSpaceManager {
  public:
    Thread** alloTable;
    void dumpState();
    static AddrSpaceManager* getInstance();
    int findAndTakeFreePage();
  private:
    static AddrSpaceManager* instance;
    AddrSpaceManager(int pageNum);
};

#endif // ADDRSPACE_H
