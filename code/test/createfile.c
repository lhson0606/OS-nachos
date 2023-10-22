#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(){
    // int len;
    // char buffer[MaxStrnLength];
    // int fileID;

    // int read_count = Read(buffer, 100, 0);
    // PrintStrn(buffer);

    char* filename = "file1.txt";
    int fileID;
    int charWritten;

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

    charWritten = Write("Hello world", 255, fileID);

    if(charWritten != -1){
        PrintStrn("Write to file1 success\n");
    }else{
        PrintStrn("Can't write to file1\n");
        Halt();
    }

    if(Close(fileID) == 0){
        PrintStrn("File1 closed\n");
    }else{
        PrintStrn("Can't close file1\n");
        Halt();
    }
    // PrintStrn("Reading from console: ");
    // char* buffer = 0;
    // buffer = malloc(MaxStrnLength);
    // memset(buffer, 0, MaxStrnLength);
    // int read_count = Read(MaxStrnLength, 3, 0);
    // PrintStrn(buffer);

    Halt();
}
