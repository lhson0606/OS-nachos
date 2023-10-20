#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32

int main(){
    int len;
    char filename[MaxFileLength+1];

    // if(Create("file1", 0) == -1){
    //     PrintStrn("Can't create file1\n");
    // }else{
    //     PrintStrn("File created\n");
    // }
    char* hello = "Hello world\n";
    PrintStrn("Hello world 1\n");
    Create("file1", 0);
    PrintStrn("Hello world 2\n");
    PrintStrn("Hello world 3\n");
    PrintStrn("Hello world 4\n");

    Halt();
}
