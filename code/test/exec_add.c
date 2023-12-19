#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(int argc, char **argv){
    int proId1 = -1;
    int proId2 = -1;
    char* argsPro1[] =  {"test/cat", "hello.txt"};
    char* argsPro2[] =  {"test/copy", "a.txt", "b.txt"};

    proId1 = ExecV(2, argsPro1);
    proId2 = ExecV(3, argsPro2);

    Join(proId1);
    Join(proId2);

    Exit(0);
}