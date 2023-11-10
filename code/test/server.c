#include "syscall.h"
#include "copyright.h"

int main(){
    int result = -1;
    int ssID = -1;
    int connect_result = -1;
    char* msg = "Hello from client";
    int send_result = -1;

    ssID = ServerCreate(8888);
    if(ssID == -1){
        PrintStrn("Create server fail\n");
    }else{
        PrintStrn("Create server success\n");
    }

    connect_result = ServerListen(ssID);

    //will not reach here
    Halt();
}

