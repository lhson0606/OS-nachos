#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32

int main(){
    int len;
    char filename[] = "file1";
    int fileID;

    if(Create(filename, 0) == 0){    
        PrintStrn("File created\n");           
    }else{
        PrintStrn("Can't create file1\n");
        Halt();
    }

    fileID = Open(filename, 0);
    
    if(fileID != -1){
        PrintStrn("File opened\n");
    }else{
        PrintStrn("Can't open file1\n");
        Halt();
    }

    if(Close(fileID) == 0){
        PrintStrn("File1 closed\n");
    }else{
        PrintStrn("Can't close file1\n");
        Halt();
    }

    Halt();
}
