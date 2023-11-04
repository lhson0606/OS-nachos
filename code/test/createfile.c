#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    //create file
    int create_result = -1;
    char* fileName = "hello.txt";

    create_result = Create(fileName);

    if(create_result == -1){
        PrintStrn("Create file hello.txt fail\n");
    }else{
        PrintStrn("Create file hello.txt success\n");
    }

    Halt();
}
