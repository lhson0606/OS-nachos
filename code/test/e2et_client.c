#include "syscall.h"
#include "copyright.h"
#include "user_utils.h"

int main(){
    int create_result;
    int connect_result;
    int write_result;
    int read_result;
    int close_result;
    int remove_result;
    int saved_fd;
    int socket_id_array[18];
    char* random_content = "Hello world";
    char read_buffer[MaxBufferLength];
    int i = 0;
    //*****************************************************************************************//
    Write("\n\n****Socket creation test****\n", MaxBufferLength, ConsoleOutput);
    //test SocketTCP()
    
    create_result = SocketTCP();
    if(create_result != -1){
        //PrintStrn("\tCan create socket: \r\t\t\t\t\t\t\t\t\t\tpassed\n");
        Write("\tCan create socket: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferLength, ConsoleOutput);
    }
    else{
       //PrintStrn("\tCan create socket: \r\t\t\t\t\t\t\t\t\t\tfailed\n");
       Write("\tCan create socket: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferLength, ConsoleOutput);
    }

    //test Connect()
    //Cannot connect if server is not open
    connect_result = Connect(create_result, "127.0.0.1", 8888);

    if(connect_result != -1){
        PrintStrn("\tCan connect to server:  \r\t\t\t\t\t\t\t\t\t\tpassed\n");
    }
    else{
        PrintStrn("\tCan connect to server:  \r\t\t\t\t\t\t\t\t\t\tfailed\n");
    }

    //test Write()
    write_result = Send(create_result, random_content, MaxBufferLength);
    //test Read()
    read_result = Receive(create_result, read_buffer, MaxBufferLength);
    read_buffer[read_result] = '\0';
    PrintStrn(read_buffer);

    Halt();
}