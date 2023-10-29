#include "syscall.h"
#include "copyright.h"

int main(){
    int socketID;
    int connect_result = -1;
    char* msg = "Hello from client";
    int send_result = -1;


    /*---------------------------------------------------------*/
    socketID = SocketTCP();

    // if(socketID != -1){
    //     PrintStrn("Socket created\n");
    // }else{
    //     PrintStrn("Can't create socket\n");
    //     Halt();
    // }

    /*---------------------------------------------------------*/
    connect_result = Connect(socketID, "172.24.99.181", 8889);

    if(connect_result != -1){
        //PrintStrn("Connect success\n");
    }else{
        //PrintStrn("Connect fail\n");
        Halt();
    }

    /*---------------------------------------------------------*/
    send_result = Send(socketID, msg, 17);

    if(send_result != -1){
        PrintStrn("Send success\n");
    }else{
        PrintStrn("Send fail\n");
        Halt();
    }

    Halt();
}