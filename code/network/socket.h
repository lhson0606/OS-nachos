#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "openstream.h"
#include "openfile.h"
#include "main.h"


int closeSocket(int socket);
int unixSocketWrite(int socket, char* buffer, int size);
int unixSocketRead(int socket, char* buffer, int size);

class Socket : public OpenStream
{
private:

    bool isSending = false;
    int socket_fd;
    int pos;
    char ip_address[32];
    int port;
    int type;
    int protocol;
    int status;
    int const MaxMailSize = 1024;

public:
    Socket(){
        
    };

    int open() override{
        return 0;
    }

    int read(char* buffer, int size) override{
        return unixSocketRead(socket_fd, buffer, size);
    };

    int write(char* buffer, int size) override{
        int res = unixSocketWrite(socket_fd, buffer, size);
        return res;
    };
    int seek(int pos) override{return -1;};
    int close() override{
        int res = closeSocket(socket_fd);
        socket_fd = -1;
        return res;
    };
    bool canRead() override{return true;};
    bool canWrite() override{return true;};
    int length() override{return -1;};
    void getName(char* buffer, int size){
        strncpy(buffer, ip_address, size);
    };

    int connect_s(char* ip, int port){

        int status;
        struct sockaddr_in serv_addr;

        DEBUG(dbgNet, "\n\tConnecting to " << ip << ":" << port);
        strncpy(ip_address, ip, MAX_FILE_NAME_LENGTH);
        this->port = port;

        if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
        { 
            DEBUG(dbgNet, "\n\tSocket creation error"); 
            return -1; 
        }

        DEBUG(dbgNet, "\n\tSocket created "<< socket_fd);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)  
        { 
            DEBUG(dbgNet, "\n\tInvalid address/ Address not supported"); 
            return -1; 
        }else{
            DEBUG(dbgNet, "\n\tAddress " << ip_address << " is valid");
        }

        if((status = connect(socket_fd, (struct sockaddr*)&serv_addr,  sizeof(serv_addr))) < 0)  
        { 
            DEBUG(dbgNet, "\n\tConnection Failed " << status); 
            return -1; 
        }else{
            DEBUG(dbgNet, "\n\tConnected to " << ip_address << ":" << port);
        }

        return socket_fd;
    }


    ~Socket(){
        if(socket_fd != -1){
            close();
        }
        
    };
};

#endif // SOCKET_H