#include "syscall.h"
#include "copyright.h"
#define MaxStrnLength 255
#define MaxFileNameLength 32

int main(int argc, char **argv){
    //create file
    int create_result = -1;
    char fileName[MaxFileNameLength];
    char argvs[2][100];
    int argsRes = -1;
    argsRes = GetArgvs(1, argvs, 100);

    if (argsRes == -1 ){
        PrintStrn("Fail to read arguments!!");
        Halt();
    }

    create_result = Create(argvs[0]);

    if (create_result == -1){
        PrintStrn("Create file fail\n");
    } else {
        PrintStrn("Create file success\n");
    }

    Halt();
}
