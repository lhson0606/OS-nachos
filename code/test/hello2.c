#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int i = 0;
    int semCreateResult = -1;

    semCreateResult = CreateSemaphore("sem2", 0);
    Wait("sem1");

    for(i = 0; i<10; i++){

        PrintStrn("Hello from thread 2\n");

        if(i == 5){
            Signal("sem2");
        }
    }

    PrintStrn("From thread 2: I'm dead :P\n");
    Exit(0);
}
