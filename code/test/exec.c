#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int i = 0;

    Exec("test/test1");
    Exec("test/test2");

    Exit(0);
}