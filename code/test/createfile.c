#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    //create file
    int fileID = -1;
    char* fileName = "hello.txt";

    fileID = Create(fileName);

    if(fileID == -1){
        PrintStrn("Create file hello.txt fail\n");
    }else{
        PrintStrn("Create file hello.txt success\n");
    }

    Halt();
}
