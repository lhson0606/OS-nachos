#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    char buffer[MaxStrnLength];
    int fileID = -1;
    int open_result;
    int read_result;
    char argvs[10][MaxStrnLength];
    int result = -1;
    result = GetArgvs(1, argvs, MaxStrnLength);

    fileID = Open(argvs[0], 1);

    if (fileID == -1){
        PrintStrn("Can not open file\n");
        Halt();
    }else{
        PrintStrn("Open file successfully\n");
    }

    read_result = Read(buffer, MaxStrnLength, fileID);
    buffer[read_result] = '\0';

    if (read_result == -1){
        PrintStrn("Can not read file\n");
        Halt();
    }else{
        PrintStrn("Read file successfully\n");
        PrintStrn(buffer);
    }

    Halt();
}
