#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int i = 0;

    Exec("test/hello1");
    Exec("test/hello2");

    Exit(0);
}