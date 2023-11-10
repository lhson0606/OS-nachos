#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

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
    char buffer[MaxBufferLength];
    /*---------------------------------------------------------*/
    socketID = SocketTCP();

    if(socketID != -1){
        PrintStrn("Socket created\n");
    }else{
        PrintStrn("Can't create socket\n");
        Halt();
    }

    /*---------------------------------------------------------*/
    PrintStrn("Waiting for server\n");

    while(connect_result<0){
        connect_result = Connect(socketID, "127.0.0.1", 8888);
    }    

    if(connect_result != -1){
        PrintStrn("Connect success\n");
    }else{
        PrintStrn("Connect fail\n");
        Halt();
    }
    /*---------------------------------------------------------*/
    send_result = Send(socketID, msg, MaxBufferLength);

    if(send_result != -1){
        PrintStrn("Send success: ");
        PrintStrn(msg);
        PrintStrn("\n");
    }else{
        PrintStrn("Send fail\n");
        Halt();
    }

    /*---------------------------------------------------------*/
    memeset(buffer, MaxBufferLength, 0);
    
    read_result = Receive(socketID, buffer, MaxBufferLength);

    if(read_result != -1){
        PrintStrn("Receive success: ");
        PrintStrn(buffer);
        PrintStrn("\n");
    }else{
        PrintStrn("Receive fail\n");
        Halt();
    }
}

void printIndex(int index){
    switch(index){
        case 0:
        {
            PrintStrn("0");
            break;
        }
        case 1:
        {
            PrintStrn("1");
            break;
        }
        case 2:
        {
            PrintStrn("2");
            break;
        }
        case 3:
        {
            PrintStrn("3");
            break;
        }
        case 4:
        {
            PrintStrn("4");
            break;
        }
    }
}

int main(){
    int i = 0;

    for(i = 0; i<4; i++){
        PrintStrn("\n\n****Socket [");
        printIndex(i);
        PrintStrn("]****\n");
        doTest();
    }    

    Halt();
}