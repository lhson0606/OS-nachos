#include "socket.h"
#include "copyright.h"
#include <cstring>
#include <stdio.h>

int closeSocket(int socket){
    return close(socket);
}

int unixSocketWrite(int socket, char* buffer, int size){
    int res = write(socket, buffer, size);
    if(res < 0){
        DEBUG(dbgNet, "\n\tError writing to socket " << socket);
    }
    return res;

}

int unixSocketRead(int socket, char* buffer, int size){
    int res = read(socket, buffer, size);
    if(res < 0){
        DEBUG(dbgNet, "\n\tError reading from socket " << socket);
    }
    return res;
}