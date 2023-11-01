#include "syscall.h"
#include "copyright.h"

void memeset(char* buffer, int size, char value){
    int i = 0;
    for(i = 0; i < size; i++){
        buffer[i] = value;
    }
}

void doTest(){
    int result = -1;
    int socketID;
    int connect_result = -1;
    char* msg = "Hello from client";
    int send_result = -1;
    int read_result = -1;
    char buffer[255];
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
    /*---------------------------------------------------------*/
    send_result = Send(socketID, msg, 17);

    if(send_result != -1){
        PrintStrn("Send success\n");
    }else{
        PrintStrn("Send fail\n");
        Halt();
    }

    /*---------------------------------------------------------*/
    memeset(buffer, 255, 0);
    
    read_result = Receive(socketID, buffer, 17);

    if(read_result != -1){
        PrintStrn("Receive success\n");
        PrintStrn(buffer);
    }else{
        PrintStrn("Receive fail\n");
        Halt();
    }
}

int main(){
    int i = 0;

    for(i = 0; i < 10; i++){
        doTest();
    }

    
}