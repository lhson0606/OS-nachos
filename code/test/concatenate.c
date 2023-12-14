#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){

    int fd_a, fd_b;
    int fd_concatenate_result;

    char filename_concatenate[] = "concatenate.txt";

    int read_result;

    char buffer[MaxBufferLength];
    char argvs[MaxArgvs][MaxArgvLength];
    int argsRes = -1;
    argsRes = GetArgvs(2, argvs, MaxArgvLength);

    if (argsRes == -1 ) {
        PrintStrn("Fail to read arguments!!\n");
        Halt();
    }

    Create(filename_concatenate);
    fd_concatenate_result = Open(filename_concatenate, RW);

    if ( fd_concatenate_result == -1 ) {
        PrintStrn("Can not open file concatenate.txt\n");
        Halt();
    }

    fd_a = Open(argvs[0], RW);

    if (fd_a == -1){
        PrintStrn("Can not open file a.txt\n");
        Halt();
    }

    fd_b = Open(argvs[1], RW);

    if (fd_b == -1){
        PrintStrn("Can not open file ");
        PrintStrn(argvs[1]);
        PrintStrn("\n");
        Halt();
    }

    read_result = Read(buffer, MaxBufferLength, fd_a);

    if(read_result == -1){
        PrintStrn("Can not read file \n");
        PrintStrn(argvs[0]);
        PrintStrn("\n");
        Halt();
    }

    PrintStrn("\nContent of ");
    PrintStrn(argvs[0]);
    PrintStrn(":\n");
    PrintStrn(buffer);
    Write(buffer, read_result, fd_concatenate_result);

    read_result = Read(buffer, MaxBufferLength, fd_b);

    if(read_result == -1){
        PrintStrn("Can not read file b.txt\n");
        Halt();
    }

    PrintStrn("\nContent of ");
    PrintStrn(argvs[1]);
    PrintStrn(":\n");
    PrintStrn(buffer);
    Write(buffer, read_result, fd_concatenate_result);
    
    PrintStrn("\nConcatenate file ");
    PrintStrn(argvs[0]);
    PrintStrn(" and ");
    PrintStrn(argvs[1]);
    PrintStrn(" to \"concatenate.txt\" successfully\n");

    Halt();
}
