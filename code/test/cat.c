#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    char buffer[MaxStrnLength];
    int fileID = -1;
    int open_result;
    int read_result;
    char argvs[2][100];
    int result = -1;
    result = GetArgvs(2, argvs, 100);

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
    } else {
        PrintStrn("Read file successfully\n");
        PrintStrn("File content: \n");
        PrintStrn(buffer);
        PrintStrn("\n");
    }

    Halt();
}