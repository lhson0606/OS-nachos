#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    char buffer[MaxStrnLength];
    int fileID;
    int desfileID;
    int read_result;
    char argvs[MaxArgvs][MaxArgvLength];
    int argsRes = -1;
    argsRes = GetArgvs(2, argvs, MaxArgvLength);

    if (argsRes == -1 ) {
        PrintStrn("Fail to read arguments!!\n");
        Halt();
    }

    PrintStrn("Received source file path: ");
    PrintStrn(argvs[0]);
    PrintStrn("\n");
    PrintStrn("Received destination file path: ");
    PrintStrn(argvs[1]);
    PrintStrn("\n");

    fileID = Open(argvs[0], RO);
    desfileID = Open(argvs[1], RW);

    if (desfileID == -1) {
        PrintStrn("Can not open destination file\n");
        Halt();
    } else{
        PrintStrn("Destination file opened successfully\n");
    }

    if (fileID == -1) {
        PrintStrn("Can not open source file\n");
        Halt();
    } else {
        PrintStrn("Source file opened successfully\n");
    }

    read_result = Read(buffer, MaxBufferLength, fileID);
    buffer[read_result] = '\0';

    if (read_result == -1) {
        PrintStrn("Can not read file\n");
        Halt();
    } else {
        PrintStrn("File copied successfully\n");
        Write(buffer,read_result,desfileID);
    }

    Exit(0);
}
