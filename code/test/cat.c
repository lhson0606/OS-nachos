#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    char buffer[MaxStrnLength];
    int fileID = -1;
    int open_result;
    int read_result;
    char argvs[MaxArgvs][MaxArgvLength];
    int result = -1;
    result = GetArgvs(2, argvs, MaxArgvLength);

    PrintStrn("Received file path: ");
    PrintStrn(argvs[0]);
    PrintStrn("\n");
    fileID = Open(argvs[0], RO);

    if (fileID == -1){
        PrintStrn("Can not open file\n");
        Exit(-1);
    }else{
        PrintStrn("Open file successfully\n");
    }

    read_result = Read(buffer, MaxStrnLength, fileID);
    buffer[read_result] = '\0';

    if (read_result == -1){
        PrintStrn("Can not read file\n");
        Exit(-1);
    } else {
        PrintStrn("Read file successfully\n");
        PrintStrn("File content: \n");
        PrintStrn(buffer);
        PrintStrn("\n");
    }

    Exit(0);
}