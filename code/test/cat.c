#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    char filename[] = "hello.txt";
    char buffer[MaxStrnLength];
    int fileID;
    int open_result;
    int read_result;

    fileID = Open(filename, 0);

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
