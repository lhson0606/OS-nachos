#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    char count[] = "0";
    Exec("test/hello1");
    Exec("test/hello2");
    Exec("test/hello1");
    Exec("test/hello2");
    while(1);

    Halt();
}