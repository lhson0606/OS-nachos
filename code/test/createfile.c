#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    //create file
    int create_result = -1;
    char fileName[MaxFileNameLength];
    char argvs[MaxArgvs][MaxArgvLength];
    int argsRes = -1;
    argsRes = GetArgvs(1, argvs, MaxArgvLength);

    if (argsRes == -1 ){
        PrintStrn("Fail to read arguments!!");
        Halt();
    }

    create_result = Create(argvs[0]);
    PrintStrn("Create file \"");
    PrintStrn(argvs[0]);
    PrintStrn("\" result: ");

    if (create_result == -1){
        PrintStrn("failed\n");
    } else {
        PrintStrn("success\n");
    }

    Halt();
}
