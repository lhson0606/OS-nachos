#include "syscall.h"
#include "copyright.h"

#define MaxBufferSize 1024
#define ConsoleInput 0
#define ConsoleOutput 1


int main(){
    int create_result;
    
    create_result = ServerCreate(8888);
    //*****************************************************************************************//
    PrintStrn("\n\n****Socket creation test****\n");
    if(create_result != -1){
        PrintStrn("\tCan create server socket: \r\t\t\t\t\t\t\t\t\t\tpassed\n");
    }
    else{
       PrintStrn("\tCan create server socket: \r\t\t\t\t\t\t\t\t\t\tfailed\n");
    }

    //run server, it will be blocked forever here
    ServerListen(create_result);

    Halt();
}