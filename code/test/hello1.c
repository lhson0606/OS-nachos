#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int i = 0;
    int semCreateResult = -1;

    semCreateResult = CreateSemaphore("sem1", 0);

    if(semCreateResult == -1){
        PrintStrn("Error creating semaphore\n");
        Exit(0);
    }

    for(i = 0; i<10; i++){
        PrintStrn("Hello from thread 1\n");

        if(i == 5){
            Signal("sem1"); 
            Wait("sem2");
        }
    }

    PrintStrn("From thread 1: I'm dead :P\n");
    Exit(0);
}
