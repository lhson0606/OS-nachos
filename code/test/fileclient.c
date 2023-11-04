#include "syscall.h"
#include "copyright.h"

#define MaxBufferSize 1024

int main(){
    int result = -1;
    int socketID;
    int connect_result = -1;
    int send_result = -1;
    int read_result = -1;
    int aID = -1;
    int bID = -1;
    int write_result = -1;
    int create_result = -1;
    char buffer[1024];
    char file_a[] = "a.txt";
    char file_b[] = "b.txt";

    /*---------------------------------------------------------*/
    socketID = SocketTCP();

    if(socketID != -1){
        PrintStrn("Socket created\n");
    }else{
        PrintStrn("Can't create socket\n");
        Halt();
    }

    connect_result = Connect(socketID, "127.0.0.1", 8888);

    if(connect_result != -1){
        PrintStrn("Connect success to 127.0.0.1:8888\n");
    }else{
        PrintStrn("Connect fail\n");
        Halt();
    }

    aID = Open(file_a, 1);

    if(aID != -1){
        PrintStrn("Open file a.txt success\n");
    }else{
        PrintStrn("Open file fail\n");
        Halt();
    }

    read_result = Read(buffer, MaxBufferSize, aID);
    buffer[read_result] = '\0';

    if(read_result != -1){
        PrintStrn("Read file a.txt success\n");
    }else{
        PrintStrn("Read file a.txt fail\n");
        Halt();
    }

    PrintStrn("Content of file a.txt:\n");
    PrintStrn(buffer);

    write_result = Write(buffer, MaxBufferSize, socketID);

    if(write_result != -1){
        PrintStrn("Write to server success\n");
    }else{
        PrintStrn("Write to server fail\n");
        Halt();
    }

    read_result = Receive(socketID, buffer, MaxBufferSize);

    if(read_result != -1){
        PrintStrn("Receive from server success\n");
    }else{
        PrintStrn("Receive from server fail\n");
        Halt();
    }

    PrintStrn("Content from server:\n");
    PrintStrn(buffer);
    PrintStrn("\n");

    create_result = Create(file_b);

    if(create_result != -1){
        PrintStrn("Create file b.txt success\n");
    }else{
        PrintStrn("Create file fail\n");
        Halt();
    }

    bID = Open(file_b, 1);

    if(bID != -1){
        PrintStrn("Open file b.txt success\n");
    }else{
        PrintStrn("Open file fail\n");
        Halt();
    }

    buffer[read_result] = '\0';
    write_result = Write("hello", 5, bID);

    if(write_result != -1){
        PrintStrn("Write to file b.txt success\n");
    }else{
        PrintStrn("Write to file fail\n");
        Halt();
    }

    //close all
    Close(aID);
    Close(bID);
    Close(socketID);      

    //will not reach here
    Halt();
}

