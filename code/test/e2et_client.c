#include "syscall.h"
#include "copyright.h"

#define MaxBufferSize 1024
#define ConsoleInput 0
#define ConsoleOutput 1

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
    char read_buffer[MaxBufferSize];
    int i = 0;
    //*****************************************************************************************//
    //test SocketTCP()
    Write("\n\n****Socket creation test****\n", MaxBufferSize, ConsoleOutput);
    create_result = SocketTCP();
    if(create_result != -1){
        PrintStrn("\tCan create socket: \r\t\t\t\t\t\t\t\t\t\tpassed\n");
    }
    else{
       PrintStrn("\tCan create socket: \r\t\t\t\t\t\t\t\t\t\tfailed\n");
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


    Halt();
}