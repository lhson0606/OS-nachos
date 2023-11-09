#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    char buffer[MaxStrnLength];
    int fileID;
    int desfileID;
    int read_result;
    char argvs[10][100];
    int result = -1;
    result = GetArgvs(2, argvs, 100);
    
    fileID = Open(argvs[0], 0);
    desfileID = Open(argvs[1],1);

    if (desfileID == -1){
        PrintStrn("Can not open destination file\n");
        Halt();
    }else{
        PrintStrn("Destination file opened successfully\n");
    }

    if (fileID == -1){
        PrintStrn("Can not open source file\n");
        Halt();
    }else{
        PrintStrn("Source file opened successfully\n");
    }

    read_result = Read(buffer, MaxStrnLength, fileID);
    buffer[read_result] = '\0';

    if (read_result == -1){
        PrintStrn("Can not read file\n");
        Halt();
    }else{
        PrintStrn("File copied successfully\n");
        Write(buffer,read_result,desfileID);
    }

    Halt();
}
