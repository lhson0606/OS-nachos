#include "syscall.h"
#include "copyright.h"

//command to test (stand in "code" folder) 
//build.linux/nachos -x test/mytest -argvs hello how are you -d u

int main() {
    char argvs[10][100];
    int result = -1;
    int i = 0;
    //test GetArgvs()
    result = GetArgvs(10, argvs, 100);

    for(i = 0; i < result; i++){
        PrintStrn(argvs[i]);
    }

    PrintStrn("From test: I'm dead :P\n");
    Exit(0);
}


