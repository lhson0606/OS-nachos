#ifndef _PCB_H_
#define _PCB_H_

#include "synch.h"

class PCB
{

public:
    int parentID; // The parent process’s ID
    int pID; // The process ID

    PCB(int id); // Constructor
    ~PCB(); // Destructor
    // Load the program has the name is “filename” and the process id is pid
    int Exec(char* name,int pid); //Create a thread with the name is filename and the process id is pid
    void SetArgvs(int argc, char* argv[]); // Set the arguments for the process
    int GetArgc(); // Return the number of arguments
    char** GetArgv(); // Return the arguments
    int GetID(); // Return the PID of the current process
    int GetNumWait(); // Return the number of the waiting process
    void JoinWait(); // The parent process wait for the child process finishes
    void ExitWait(); // The child process finishes
    void JoinRelease(); // The child process notice the parent process
    void ExitRelease(); // The parent process accept to exit the child process
    void IncNumWait(); // Increase the number of the waiting process
    void DecNumWait(); // Decrease the number of the waiting process
    void SetExitCode(int); // Set the exit code for the process
    int GetExitCode(); // Return the exitcode
    void SetFileName(char*); // Set the process name
    char* GetFileName(); // Return the process name
    int GetParentID(); // Return the parent process ID
    void Exit(); // Exit the process

private:
    Semaphore* joinsem; // semaphore for join process
    Semaphore* exitsem; // semaphore for exit process
    Semaphore* multex; // exclusive access semaphore
    Semaphore* shouldExit; // semaphore waiting for child process to exit
    int exitcode;
    int numwait; // the number of join process
    char* filename; // the name of the process
    char *userArgs[MAX_ARG_NUM]; // arguments of the process
    int userArgc = 0; // number of arguments of the process
};

#endif // _PCB_H_