#include "syscall.h"
#include "copyright.h"
#define MaxFileLength 32
#define MaxStrnLength 255

int main(int argc, char **argv){
    int fd_a, fd_b;
    int fd_concatenate_result;
    char filename_concatenate[] = "concatenate.txt";
    char filename_a[] = "a.txt";
    char filename_b[] = "b.txt";
    int open_result;
    int read_result;
    char buffer[MaxStrnLength];

    Create(filename_concatenate);
    fd_concatenate_result = Open(filename_concatenate, 1);

    if(fd_concatenate_result == -1){
        PrintStrn("Can not open file concatenate.txt\n");
        Halt();
    }

    fd_a = Open(filename_a, 1);

    if (fd_a == -1){
        PrintStrn("Can not open file a.txt\n");
        Halt();
    }

    fd_b = Open(filename_b, 1);

    if (fd_b == -1){
        PrintStrn("Can not open file b.txt\n");
        Halt();
    }

    read_result = Read(buffer, MaxStrnLength, fd_a);

    if(read_result == -1){
        PrintStrn("Can not read file a.txt\n");
        Halt();
    }

    PrintStrn("\nContent of a.txt:\n");
    PrintStrn(buffer);
    Write(buffer, read_result, fd_concatenate_result);

    read_result = Read(buffer, MaxStrnLength, fd_b);

    if(read_result == -1){
        PrintStrn("Can not read file b.txt\n");
        Halt();
    }

    PrintStrn("\nContent of b.txt:\n");
    PrintStrn(buffer);
    Write(buffer, read_result, fd_concatenate_result);

    PrintStrn("\nConcatenate file a.txt and b.txt to concatenate.txt successfully\n");

    Halt();
}
