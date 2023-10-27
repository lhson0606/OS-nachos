#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(){
    char* filename = "file1.txt";
    int fileID;
    int charWritten;
    int read_count;
    char buffer[255];
    int seek_position = 0;
    int seek_result = -1;

    /*---------------------------*/
    if(Create(filename, 0) == 0){    
        PrintStrn("File created\n");           
    }else{
        PrintStrn("Can't create file1\n");
        Halt();
    }

    /*---------------------------*/
    fileID = Open(filename, 1);
    
    if(fileID != -1){
        PrintStrn("File opened\n");
    }else{
        PrintStrn("Can't open file1\n");
        Halt();
    }

    // /*---------------------------*/
    charWritten = Write("Hello world", 255, fileID);

    if(charWritten != -1){
        PrintStrn("Write to file1 success\n");
    }else{
        PrintStrn("Can't write to file1\n");
        Halt();
    } //bugs here

    /*---------------------------*/
    read_count = Read(buffer, 255, fileID);
    buffer[read_count+1] = '\0';

    if(read_count != -1){
        PrintStrn("Read from file1 success\n");
        PrintStrn(buffer);
    }else{
        PrintStrn("Can't read from file1\n");
        Halt();
    }

    /*---------------------------*/
    
    
    seek_result = Seek(-1, fileID);

    /*---------------------------*/
    read_count = Read(buffer, 255, fileID);
    buffer[read_count+1] = '\0';

    if(read_count != -1){
        PrintStrn("Read from file1 success\n");
        PrintStrn(buffer);
    }else{
        PrintStrn("Can't read from file1\n");
        Halt();
    }
    


    // /*---------------------------*/
    if(Close(fileID) == 0){
        PrintStrn("File1 closed\n");
    }else{
        PrintStrn("Can't close file1\n");
        Halt();
    }

    /*---------------------------*/
    if(Remove(filename) == 0){
        PrintStrn("File1 removed\n");
    }else{
        PrintStrn("Can't remove file1\n");
        Halt();
    }

    Halt();
}
