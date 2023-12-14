#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int delete_result;
    char argvs[2][MaxArgvLength];
    int argsRes = -1;
    int openRes = - 1;
    argsRes = GetArgvs(1, argvs, MaxArgvLength);

    if (argsRes == -1 ) {
        PrintStrn("Fail to read arguments!!\n");
        Halt();
    }

    PrintStrn("Deleting file: ");
    PrintStrn(argvs[0]);
    PrintStrn("\n");
    
    delete_result = Remove(argvs[0]);

    if (delete_result == -1){
        PrintStrn("Can not delete file\n");
        Halt();
    }else{
        PrintStrn("Delete file successfully\n");
    }

    Halt();
}
