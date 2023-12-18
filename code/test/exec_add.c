#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int i = 0;
    int pA = -1;
    int pB = -1;

    pA = Exec("test/hello1");
    pB = Exec("test/hello2");

    //#todo: figure out why when main uses busy waiting cuncurrency won't work
    Join(pA);
    Join(pB);
    PrintStrn("From main: I'm dead :P\n");

    Exit(0);
    Halt();
}