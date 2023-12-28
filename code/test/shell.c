#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

//#todo put this in another file
const char* EXIT_CMD = "exit";

void readShellCmd(char* msg);

void printShellMsg(char* msg);

int shouldExit(const char* msg);

int strnCmp(const char* s1, const char* s2);

int
main()
{
    char cmdBuff[MaxStrnLength];
    cmdBuff[0] = '\0';

    while(!shouldExit(cmdBuff)){
        readShellCmd(cmdBuff);
        printShellMsg(cmdBuff);
    }

    Exit(0);
}

void readShellCmd(char* msg){
    int size;
    //#todo console input string size > 64 will result in a multi-chunk read (each is 64), might want to do something about that
    PrintStrn("\nShell> ");
    size = Read(msg, MaxStrnLength, ConsoleInput);
    msg[size] = '\0';
}

void printShellMsg(char* msg){
    PrintStrn(msg);
}

int shouldExit(const char* msg){
    return strnCmp(msg, EXIT_CMD) == 0;
}

int strnCmp(const char* s1, const char* s2){
    int i = 0;
    while(s1[i] != '\0' && s2[i] != '\0'){
        if(s1[i] != s2[i]){
            return s1[i] - s2[i];
        }
        i++;
    }
    return s1[i] - s2[i];
}