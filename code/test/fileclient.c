#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

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
    char buffer[MaxBufferLength];

    char argvs[2][MaxStrnLength];
    int res = -1;
    res = GetArgvs(2, argvs, MaxStrnLength);

    if (res == -1 ) {
        PrintStrn("Fail to read arguments!!\n");
        Halt();
    }

    PrintStrn("Recieved file paths: ");
    PrintStrn(argvs[0]);
    PrintStrn(" ");
    PrintStrn(argvs[1]);
    PrintStrn("\n");

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

    aID = Open(argvs[0], RW);

    if(aID != -1){
        PrintStrn("Open file ");
        PrintStrn(argvs[0]);
        PrintStrn(" success\n");
    }else{
        PrintStrn("Open file ");
        PrintStrn(argvs[0]);
        PrintStrn(" fail\n");
        Halt();
    }

    read_result = Read(buffer, MaxBufferLength, aID);
    buffer[read_result] = '\0';

    if(read_result != -1){
        PrintStrn("Read file ");
        PrintStrn(argvs[0]);
        PrintStrn(" success\n");
    }else{
        PrintStrn("Read file ");
        PrintStrn(argvs[0]);
        PrintStrn(" fail\n");
        Halt();
    }

    PrintStrn("Content of ");
    PrintStrn(argvs[0]);
    PrintStrn(":\n");
    PrintStrn(buffer);
    PrintStrn("\n");

    write_result = Write(buffer, MaxBufferLength, socketID);

    if(write_result != -1){
        PrintStrn("Write to server success\n");
    }else{
        PrintStrn("Write to server fail\n");
        Halt();
    }

    read_result = Receive(socketID, buffer, MaxBufferLength);

    if(read_result != -1){
        PrintStrn("Receive from server success\n");
    }else{
        PrintStrn("Receive from server fail\n");
        Halt();
    }

    PrintStrn("Content from server:\n");
    PrintStrn(buffer);
    PrintStrn("\n");

    create_result = Create(argvs[1]);

    if(create_result != -1){
        PrintStrn("Create file ");
        PrintStrn(argvs[1]);
        PrintStrn(" success\n");
    }else{
        PrintStrn("Create file ");
        PrintStrn(argvs[1]);
        PrintStrn(" fail\n");
        Halt();
    }

    bID = Open(argvs[1], RW);

    if(bID != -1){
        PrintStrn("Open file ");
        PrintStrn(argvs[1]);
        PrintStrn(" success\n");
    }else{
        PrintStrn("Open file ");
        PrintStrn(argvs[1]);
        PrintStrn(" fail\n");
        Halt();
    }

    buffer[read_result] = '\0';
    write_result = Write(buffer, read_result, bID);

    if(write_result != -1){
        PrintStrn("Write to file ");
        PrintStrn(argvs[1]);
        PrintStrn(" success\n");
    }else{
        PrintStrn("Write to file ");
        PrintStrn(argvs[1]);
        PrintStrn(" fail\n");
        Halt();
    }

    //close all
    Close(aID);
    Close(bID);
    Close(socketID);
    
    //will not reach here
    Halt();
}

