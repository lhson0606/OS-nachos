#include "syscall.h"
#include "copyright.h"

int main(){
    int result = -1;
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
    connect_result = Connect(socketID, "127.0.0.1", 8888);

    if(connect_result != -1){
        PrintStrn("Connect success\n");
    }else{
        PrintStrn("Connect fail\n");
        Halt();
    }

    // /*---------------------------------------------------------*/
    // send_result = Send(socketID, msg, 17);

    // if(send_result != -1){
    //     PrintStrn("Send success\n");
    // }else{
    //     PrintStrn("Send fail\n");
    //     Halt();
    // }

    /*---------------------------------------------------------*/
    //result = Disconnect(0);

    // if(result != -1){
    //     PrintStrn("Disconnect success\n");
    // }else{
    //     PrintStrn("Disconnect fail\n");
    //     Halt();
    // }

    Halt();
}