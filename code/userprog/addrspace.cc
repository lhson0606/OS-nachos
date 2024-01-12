// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -n -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you are using the "stub" file system, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "addrspace.h"
#include "machine.h"
#include "noff.h"
#include "kernel.h"
#include "bitmap.h"
#include "synch.h"

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
    noffH->noffMagic = WordToHost(noffH->noffMagic);
    noffH->code.size = WordToHost(noffH->code.size);
    noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
#ifdef RDATA
    noffH->readonlyData.size = WordToHost(noffH->readonlyData.size);
    noffH->readonlyData.virtualAddr = 
           WordToHost(noffH->readonlyData.virtualAddr);
    noffH->readonlyData.inFileAddr = 
           WordToHost(noffH->readonlyData.inFileAddr);
#endif 
    noffH->initData.size = WordToHost(noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost(noffH->uninitData.size);
    noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);

#ifdef RDATA
    DEBUG(dbgAddr, "code = " << noffH->code.size <<  
                   " readonly = " << noffH->readonlyData.size <<
                   " init = " << noffH->initData.size <<
                   " uninit = " << noffH->uninitData.size << "\n");
#endif
}

/**
 * This is the constructor for the address space for a user program
 * find and allocate the needed pages for the address space, this is used for multiprogramming
 * NOTE: this function won't close the executable file, the called shouldn't close it either
 * it will be closed in the destructor 
 * @param executable the executable file contains the program we are going to run
*/
AddrSpace::AddrSpace(OpenFile* executable)//#todo fix this so that it can open executable file on its own, reommend to pass in the file name
{
    DEBUG(dbgAddr, "Creating new address space for " << (int)executable);
    this->executable = executable;
    NoffHeader noffH;
    unsigned int size;
    //our executable is the file we are going to run
    ASSERT(executable != NULL);

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    DEBUG(dbgAddr, "code size: " << noffH.code.size);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    //we will try to calculate the needed size of the address space
    size = noffH.code.size + noffH.readonlyData.size + noffH.initData.size +
           noffH.uninitData.size + UserStackSize;

    DEBUG(dbgAddr, "noffH.code.size: " << noffH.code.size);
    DEBUG(dbgAddr, "noffH.readonlyData.size: " << noffH.readonlyData.size);
    DEBUG(dbgAddr, "noffH.initData.size: " << noffH.initData.size);
    DEBUG(dbgAddr, "noffH.uninitData.size: " << noffH.uninitData.size);
    DEBUG(dbgAddr, "UserStackSize: " << UserStackSize);
    DEBUG(dbgAddr, "Total size: " << size);

    numPages = divRoundUp(size, PageSize);
    pageTable = new TranslationEntry[numPages];
    size = numPages * PageSize;

    kernel->addrLock->P();
    for(int i = 0; i < numPages; i++)
    {
        int temp = kernel->gPhysPageBitMap->FindAndSet();
        //#todo: right now we are not handling the case when there is no free page
        //oh boy, we are in trouble
        ASSERT(temp != -1);
        DEBUG(dbgAddr, "Allocating physical page " << temp << " for virtual page " << i);
        //else this page is free, we will use it
        pageTable[i].virtualPage = i;
        pageTable[i].physicalPage = temp;//map the virtual page to the physical page
        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;
    }
    kernel->addrLock->V();

    //we will zero out the entire allocated pages not all the memory
    for(int i = 0; i < numPages; i++)
    {
        DEBUG(dbgAddr, "Zeroing out physical page " << pageTable[i].physicalPage);
        char* physicalAddr = &(kernel->machine->mainMemory[pageTable[i].physicalPage * PageSize]);//because each page is PageSize bytes
        bzero(physicalAddr, PageSize);
    }
}


//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//----------------------------------------------------------------------

AddrSpace::AddrSpace()
{
    pageTable = new TranslationEntry[NumPhysPages];
    for (int i = 0; i < NumPhysPages; i++) {
        DEBUG(dbgAddr, "Initializing physical page " << i);
        pageTable[i].virtualPage = i;	// for now, virt page # = phys page #
        pageTable[i].physicalPage = i;
        pageTable[i].valid = TRUE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  
    }
    
    // zero out the entire address space
    bzero(kernel->machine->mainMemory, MemorySize);
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    //unmap the physical pages
    //we will iterate through the page table and free the physical pages
    kernel->addrLock->P();

    for(int i = 0; i < numPages; i++)
    {
        DEBUG(dbgAddr, "Freeing physical page " << pageTable[i].physicalPage);
        kernel->gPhysPageBitMap->Clear(pageTable[i].physicalPage);
    }

    kernel->addrLock->V();

   delete pageTable;
}

void 
AddrSpace::LoadIntoMemory(OpenFile *executable, int startAddr, int size, int virtualAddr){
    kernel->addrLock->P();
    int pageCount = divRoundUp(size, PageSize);
    unsigned int translatedAddr;
    DEBUG(dbgAddr, "Loading " << pageCount << " pages into memory startAddr "<< startAddr << ", size " << size << ", virtualAddr" << virtualAddr);
    for(int i = 0; i < pageCount; i++){
        Translate(virtualAddr + i * PageSize, &translatedAddr, 1);
        executable->ReadAt(
		&(kernel->machine->mainMemory[translatedAddr]), 
            PageSize, startAddr + i * PageSize);
    }
    kernel->addrLock->V();
}

/**
 * This function should only be called after the constructor AddrSpace(OpenFile* executable) is called, otherwise it will not work
*/
bool
AddrSpace::Load(OpenFile* executable){
    //same as Load(char* fileName) version, but we need load the code to the correct physical page, not all the memory
    //we don't need to recalculate the size and numPages, because we already did that in the constructor
    DEBUG(dbgAddr, "Currently loading new executable with " << numPages << " pages into memory");
    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory
    DEBUG(dbgAddr, "Initializing address with: " << numPages <<" pages");

    NoffHeader noffH;
    unsigned int size;

    ASSERT(executable != NULL);

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    DEBUG(dbgAddr, "code size: " << noffH.code.size);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    //this part is changed for multiprogramming version
    /**
     * virtual addres no longer equals to    physical address. And we need to translate
     * noffH.code.virtualAddr to physical address explicitly here
     * see http://tanviramin.com/documents/nachos2.pdf?fbclid=IwAR0ykcLGxVz-uEqrS1cU7qWrpeVXhvFn9R8DEAQ6bPrmKq2poPBWpyUhaoY page 19
    */
    if (noffH.code.size > 0) {
        DEBUG(dbgAddr, "Initializing code segment.");
	    DEBUG(dbgAddr, noffH.code.virtualAddr << ", " << noffH.code.size);
        // we need to manually translate the virtual address to the physical address
        //since the main memory may not be continuous
        LoadIntoMemory(executable, noffH.code.inFileAddr, noffH.code.size, noffH.code.virtualAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG(dbgAddr, "Initializing data segment.");
	    DEBUG(dbgAddr, noffH.initData.virtualAddr << ", " << noffH.initData.size);
        LoadIntoMemory(executable, noffH.initData.inFileAddr, noffH.initData.size, noffH.initData.virtualAddr);
    }

#ifdef RDATA
    if (noffH.readonlyData.size > 0) {
        DEBUG(dbgAddr, "Initializing read only data segment.");
	    DEBUG(dbgAddr, noffH.readonlyData.virtualAddr << ", " << noffH.readonlyData.size);
        LoadIntoMemory(executable, noffH.readonlyData.inFileAddr, noffH.readonlyData.size, noffH.readonlyData.virtualAddr);
    }
#endif

    return TRUE;	
}

//----------------------------------------------------------------------
// AddrSpace::Load
// 	Load a user program into memory from a file.
//
//	Assumes that the page table has been initialized, and that
//	the object code file is in NOFF format.
//
//	"fileName" is the file containing the object code to load into memory
//----------------------------------------------------------------------

bool 
AddrSpace::Load(char *fileName) 
{
    DEBUG(dbgAddr, "Loading file " << fileName);
    OpenFile *executable = kernel->fileSystem->Open(fileName);
    NoffHeader noffH;
    unsigned int size;

    if (executable == NULL) {
	cerr << "Unable to open file " << fileName << "\n";
	return FALSE;
    }

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    DEBUG(dbgAddr, "code size: " << noffH.code.size);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

#ifdef RDATA
// how big is address space?
    size = noffH.code.size + noffH.readonlyData.size + noffH.initData.size +
           noffH.uninitData.size + UserStackSize;	
                                                // we need to increase the size
						// to leave room for the stack
    DEBUG(dbgAddr, "Total code size 1: " << size);
    DEBUG(dbgAddr, "code size: " << noffH.code.size);
    DEBUG(dbgAddr, "readonly size: " << noffH.readonlyData.size);
    DEBUG(dbgAddr, "init size: " << noffH.initData.size);
    DEBUG(dbgAddr, "uninit size: " << noffH.uninitData.size);
    DEBUG(dbgAddr, "stack size: " << UserStackSize);
#else
// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    DEBUG(dbgAddr, "Total code size 2: " << size);
    DEBUG(dbgAddr, "code size: " << noffH.code.size);
    DEBUG(dbgAddr, "readonly size: " << noffH.readonlyData.size);
    DEBUG(dbgAddr, "init size: " << noffH.initData.size);
    DEBUG(dbgAddr, "uninit size: " << noffH.uninitData.size);
    DEBUG(dbgAddr, "stack size: " << UserStackSize);
#endif
    DEBUG(dbgAddr, "Initializing address space, num pages " << numPages << ", size " << size);
    numPages = divRoundUp(size, PageSize);
    DEBUG(dbgAddr, "Initializing address space, num pages " << numPages << ", size " << size);
    size = numPages * PageSize;
    DEBUG(dbgAddr, "Initializing address space, num pages " << numPages << ", size " << size);

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG(dbgAddr, "Initializing address space: " << numPages << ", " << size);

// then, copy in the code and data segments into memory
// Note: this code assumes that virtual address = physical address
    if (noffH.code.size > 0) {
        DEBUG(dbgAddr, "Initializing code segment.");
	DEBUG(dbgAddr, noffH.code.virtualAddr << ", " << noffH.code.size);
        executable->ReadAt(
		&(kernel->machine->mainMemory[noffH.code.virtualAddr]), 
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG(dbgAddr, "Initializing data segment.");
	DEBUG(dbgAddr, noffH.initData.virtualAddr << ", " << noffH.initData.size);
        executable->ReadAt(
		&(kernel->machine->mainMemory[noffH.initData.virtualAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }

#ifdef RDATA
    if (noffH.readonlyData.size > 0) {
        DEBUG(dbgAddr, "Initializing read only data segment.");
	DEBUG(dbgAddr, noffH.readonlyData.virtualAddr << ", " << noffH.readonlyData.size);
        executable->ReadAt(
		&(kernel->machine->mainMemory[noffH.readonlyData.virtualAddr]),
			noffH.readonlyData.size, noffH.readonlyData.inFileAddr);
    }
#endif

    delete executable;			// close file
    return TRUE;			// success
}

//----------------------------------------------------------------------
// AddrSpace::Execute
// 	Run a user program using the current thread
//
//      The program is assumed to have already been loaded into
//      the address space
//
//----------------------------------------------------------------------

void 
AddrSpace::Execute() 
{

    kernel->currentThread->space = this;

    this->InitRegisters();		// set the initial register values
    this->RestoreState();		// load page table register

    kernel->machine->Run();		// jump to the user progam

    ASSERTNOTREACHED();			// machine->Run never returns;
					// the address space exits
					// by doing the syscall "exit"
}


//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    Machine *machine = kernel->machine;
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start", which
    //  is assumed to be virtual address zero
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    // Since instructions occupy four bytes each, the next instruction
    // after start will be at virtual address four.
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG(dbgAddr, "Initializing stack pointer: " << numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, don't need to save anything!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    kernel->machine->pageTable = pageTable;
    kernel->machine->pageTableSize = numPages;
}


//----------------------------------------------------------------------
// AddrSpace::Translate
//  Translate the virtual address in _vaddr_ to a physical address
//  and store the physical address in _paddr_.
//  The flag _isReadWrite_ is false (0) for read-only access; true (1)
//  for read-write access.
//  Return any exceptions caused by the address translation.
//----------------------------------------------------------------------
ExceptionType
AddrSpace::Translate(unsigned int vaddr, unsigned int *paddr, int isReadWrite)
{
    TranslationEntry *pte;
    int               pfn;
    unsigned int      vpn    = vaddr / PageSize;
    unsigned int      offset = vaddr % PageSize;

    if(vpn >= numPages) {
        return AddressErrorException;
    }

    pte = &pageTable[vpn];

    if(isReadWrite && pte->readOnly) {
        return ReadOnlyException;
    }

    pfn = pte->physicalPage;

    // if the pageFrame is too big, there is something really wrong!
    // An invalid translation was loaded into the page table or TLB.
    if (pfn >= NumPhysPages) {
        DEBUG(dbgAddr, "Illegal physical page " << pfn);
        return BusErrorException;
    }

    pte->use = TRUE;          // set the use, dirty bits

    if(isReadWrite)
        pte->dirty = TRUE;

    *paddr = pfn*PageSize + offset;

    ASSERT((*paddr < MemorySize));

    //cerr << " -- AddrSpace::Translate(): vaddr: " << vaddr <<
    //  ", paddr: " << *paddr << "\n";

    return NoException;
}




