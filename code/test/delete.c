#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    char filename[] = "hello.c";
    int delete_result;

    delete_result = Remove(filename);

    if (delete_result == -1){
        PrintStrn("Can not delete file\n");
        Halt();
    }else{
        PrintStrn("Delete file successfully\n");
    }

    Halt();
}
