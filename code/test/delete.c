#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    int delete_result;
    char argvs[2][MaxStrnLength];
    int result = -1;
    result = GetArgvs(1, argvs, MaxStrnLength);

    delete_result = Remove(argvs[0]);

    if (delete_result == -1){
        PrintStrn("Can not delete file\n");
        Halt();
    }else{
        PrintStrn("Delete file successfully\n");
    }

    Halt();
}
