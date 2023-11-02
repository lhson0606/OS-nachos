#ifndef _SERVERSOCKET_H
#define _SERVERSOCKET_H

#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h>
#include "openstream.h"
#include "main.h"

#define MAX_CLIENTS 30

class ServerSocket : public OpenStream
{
public:
    /**
     * @brief create a server socket
     * @param port
    */
    ServerSocket(int port);

    /**
     * @brief start running server
    */
    int listen();

    /**
     * @brief no used
     * @return -1
    */
    int open() override;

    /**
     * @brief read from socket
     * @param buffer
     * @param size
     * @return number of bytes read, -1 if error
    */
    int read(char* buffer, int size) override;

    /**
     * @brief write to socket
     * @param buffer
     * @param size
     * @return number of bytes written, -1 if error
    */
    int write(char* buffer, int size) override;

    /**
     * @brief no used
     * @return -1
    */
    int seek(int pos) override;

    /**
     * @brief close socket, set unix socket fd to -1
     * @return 0 if success, -1 if error
    */
    int close() override;

    /**
     * @brief no used
     * @return true
    */
    bool canRead() override;

    /**
     * @brief no used
     * @return true
    */
    bool canWrite() override;

    /**
     * @brief no used
     * @return -1
    */
    int length() override;

    /**
     * @brief no used
    */
    void getName(char* buffer, int size);

    /**
     * @brief close server socket and all client sockets
    */
    ~ServerSocket();

private:

    int ss_fd = -1;
    char ip_address[32];
    int port;;
    int const MaxMailSize = 1024;
    bool isRunning = false;
    //turn on server socket options
    int opt = true;
    //can be used to indicate which activity is happening: accept, read, write
    int activity;
    //our server socket address. will be used to bind our socket. converted from IPv4 to network byte order
    struct sockaddr_in address;
    //set of socket descriptors (unix file descriptors not us)
    fd_set readfds;
    int client_socket[MAX_CLIENTS];
    int error_code = 0;
    //max number of unix file descriptors
    int max_sd;
    int addrlen;
    
    /**
     * set up server socket, bind, listen for activity on sockets
     * @return 0 if success, -1 if error
    */
    int init();

    /**
     * @brief handle new client connection
     * @param client_sock_fd C lib socket fd (not our fd)
     * @return 0 if success, -1 if error
    */
    int handleClientNewConnection(int client_sock_fd); 

    /**
     * @brief handle client comming message
     * @param client_sock_fd C lib socket fd (not our fd)
     * @param buffer
     * @param size
     * @return 0 if success, -1 if error
    */
    int handleClientCommingMessage(int client_sock_fd, char* buffer, int size);

};

#endif // _SERVERSOCKET_H