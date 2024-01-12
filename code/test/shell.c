#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

//#todo put this in another file
const char* EXIT_CMD = "exit";

void readShellCmd(char* msg);

void printShellMsg(char* msg);

int shouldExit(const char* msg);

int strnCmp(const char* s1, const char* s2);

int executeCmd(const char* cmd);

int
main()
{
    char cmdBuff[MaxStrnLength];
    SpaceId proId = -1;

    while(1){
        readShellCmd(cmdBuff);

        if(shouldExit(cmdBuff)){
            break;
        }

        proId = -1;
        proId = executeCmd(cmdBuff);

        if(proId != -1){
            Join(proId);
        }else{
            PrintStrn("Program not found\n");
        }
    }

    Exit(0);
}

void readShellCmd(char* msg){
    int size;
    //#todo console input string size > 32 will result in a multi-chunk read (each is 64) behaviour, might want to do something about that
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

int executeCmd(const char* cmd){
    char* args[MaxArgvs];
    int argc = 0;
    int i = 0;
    int argIndex = 0;
    int proId = -1;
    args[0] = &cmd[0];

    do {
        if(cmd[i] == ' '){
            args[argc][argIndex] = '\0';
            argc++;
            argIndex = 0;
            args[argc] = &cmd[i + 1];
        }else{
            args[argc][argIndex] = cmd[i];
            argIndex++;
        }
        i++;
    }while (cmd[i] != '\0' && argc < MaxArgvs);

    //because the last arg is not followed by a space but a null terminator
    args[argc][argIndex] = '\0';
    argc++;

    proId = ExecV(argc, args);
    return proId;
}