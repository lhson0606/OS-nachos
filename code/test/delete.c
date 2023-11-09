#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    int delete_result;
    char argvs[2][MaxStrnLength];
    int argsRes = -1;
    int openRes = - 1;
    argsRes = GetArgvs(1, argvs, MaxStrnLength);

    if (argsRes == -1 ) {
        PrintStrn("Fail to read arguments!!\n");
        Halt();
    }


    openRes = Open(argvs[0], 0);

    if ( openRes == -1){
        PrintStrn("File not exists\n");
        Halt();
    }

    delete_result = Remove(argvs[0]);

    if (delete_result == -1){
        PrintStrn("Can not delete file\n");
        Halt();
    }else{
        PrintStrn("Delete file successfully\n");
    }

    Halt();
}
