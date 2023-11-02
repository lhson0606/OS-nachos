#include "socket.h"
#include "copyright.h"
#include <cstring>
#include <stdio.h>


//to distinguish between our close() and C socket close()
int sockClose(int socket){
    return close(socket);
}

//to distinguish between our wite() and C socket wite()
int sockWrite(int socket, char* buffer, int size){
    int res = write(socket, buffer, size);
    if(res < 0){
        DEBUG(dbgNet, "\n\tError writing to socket " << socket);
    }
    return res;

}

//to distinguish between our read() and C socket read()
int sockRead(int socket, char* buffer, int size){
    int res = read(socket, buffer, size);
    if(res < 0){
        DEBUG(dbgNet, "\n\tError reading from socket " << socket);
    }
    return res;
}

int sockConnect(int socket, sockaddr* serv_addr, int size){
    int res = connect(socket, serv_addr,  size);
    if(res < 0){
        DEBUG(dbgNet, "\n\tError connecting to socket " << socket);
    }
    return res;
}

Socket::Socket(){
        
};

Socket::Socket(int sock_fd){
    socket_fd = sock_fd;
};

int Socket::open(){
    return 0;
}

int Socket::read(char* buffer, int size){
    return sockRead(socket_fd, buffer, size);
};

int Socket::write(char* buffer, int size){
    int res = sockWrite(socket_fd, buffer, size);
    return res;
};

int Socket::seek(int pos){
    return -1;
};

int Socket::close(){
    int res = sockClose(socket_fd);
    socket_fd = -1;
    return res;
};

bool Socket::canRead(){
    return true;
};

bool Socket::canWrite(){
    return true;
};

int Socket::length(){
    return -1;
};

void Socket::getName(char* buffer, int size){
    strncpy(buffer, ip_address, size);
};

int Socket::connect(char* ip, int port){

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

    if((status = sockConnect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)  
    { 
        DEBUG(dbgNet, "\n\tConnection Failed " << status); 
        return -1; 
    }else{
        DEBUG(dbgNet, "\n\tConnected to " << ip_address << ":" << port);
    }

    return socket_fd;
}

int Socket::getSocketFd(){
    return socket_fd;
}

Socket::~Socket(){
    if(socket_fd != -1){
        close();
    }    
};