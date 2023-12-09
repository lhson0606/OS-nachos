#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int i = 0;

    for(i = 0; i<10; i++){
        PrintStrn("Hello from thread 1\n");
    }

    PrintStrn("From thread 1: I'm dead :P\n");
    Exit(0);
}
