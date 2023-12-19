#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int i = 0;
    int pA = -1;
    int pB = -1;
    int exitCodeA = -1;
    int exitCodeB = -1;

    pA = Exec("test/hello1");
    pB = Exec("test/hello2");
    exitCodeA = Join(pA);
    exitCodeB = Join(pB);

    if(exitCodeA == 0){
        PrintStrn("From main: pA finished with no error!\n");
    }else{
        PrintStrn("From main: pA finished with error!\n");
    }    

    if(exitCodeB == 0){
        PrintStrn("From main: pB finished with no error!\n");
    }else{
        PrintStrn("From main: pB finished with error!\n");
    }

    PrintStrn("From main: I'm dead :P\n");

    Halt();
}