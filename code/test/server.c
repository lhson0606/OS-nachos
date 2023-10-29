#include "syscall.h"
#include "copyright.h"

int main(){
    int socketID;
    int connect_result = -1;
    char* msg = "Hello from client";
    int recv_result = -1;
    char buffer[1024];


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
    while(1){
        recv_result = Receive(socketID, buffer, 1024);

        if(recv_result != -1){
            PrintStrn("Receive success\n");
            PrintStrn(buffer);
        }else{
            PrintStrn("Receive fail\n");
        }
    }
    

    Halt();
}