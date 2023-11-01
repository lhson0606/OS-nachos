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
#include "fdt.h"

#define MAX_CLIENTS 30
int runTestSocket(int port);

int unixListen(int socket, int count){
    return listen(socket, count);

}

int unixRead(int socket, char* buffer, int size){
    return read(socket, buffer, size);
}

int unixWrite(int socket, char* buffer, int size){
    return write(socket, buffer, size);
}

int unixClose(int socket){
    return close(socket);
}

class ServerSocket : public OpenStream
{
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
    
public:
    ServerSocket(int port) : OpenStream(){
        this->port = port;
        //FileDescriptorTable::getInstance()->Add(this);
    };



    int listen(){
        //runTestSocket(port);
        int new_socket;
        int i;
        int valread;

        if(init()<0){
            return error_code;
        }

        isRunning = true;

        while(isRunning){
            DEBUG(dbgNet, "\n\tServerSocket::run() - Waiting for connections...");
            
            //reset before each iteration
            FD_ZERO(&readfds);

            //add our server socket to the set, our master socket will be notified when an incoming connection arrives
            FD_SET(ss_fd, &readfds);
            max_sd = ss_fd;

            //add child sockets to set
            for(int i = 0; i < MAX_CLIENTS; i++){
                //socket descriptor
                int sd = client_socket[i];

                //if valid socket descriptor then add to read list
                if(sd > 0){
                    FD_SET(sd, &readfds);
                }

                //highest file descriptor number, need it for the select function
                if(sd > max_sd){
                    max_sd = sd;
                }
            }

            new_socket = -1;
            //wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
            activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
            DEBUG(dbgNet, "\n\tServerSocket::run() - activity: " << activity);

            if((activity < 0) && (errno != EINTR)){
                DEBUG(dbgNet, "\n\tServerSocket::run() - select error");
            }

            if(FD_ISSET(ss_fd, &readfds)){

                while(new_socket<0){
                    new_socket = accept(ss_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    DEBUG(dbgNet, "\n\tnew_socket: " << new_socket);
                    // error
                    DEBUG(dbgNet, "\n\tError - accept error: "<< errno);
                }
                

                if(new_socket < 0){
                    DEBUG(dbgNet, "\n\tServerSocket::run() - accept error");
                    return error_code = -5;
                }else{
                    DEBUG(dbgNet, "\n\tServerSocket::run() - New connection, socket fd is " << new_socket << " , ip is : " << inet_ntoa(address.sin_addr) << " , port : " << ntohs(address.sin_port));
                }
            }
            
            //add new socket to array of sockets
            for(int i = 0; i < MAX_CLIENTS; i++){
                //if position is empty
                if(client_socket[i] < 0){
                    client_socket[i] = new_socket;
                    DEBUG(dbgNet, "\n\tServerSocket::run() - Adding to list of sockets as " << i);
                    break;
                }
            }

            //else its some IO operation on some other socket
            for(i = 0; i< MAX_CLIENTS; i++){
                int sd = client_socket[i];
                char buffer [MaxMailSize];

                memset(buffer, 0, MaxMailSize);

                if(FD_ISSET(sd, &readfds)){
                    
                    valread = unixRead(sd, buffer, MaxMailSize);

                    if(valread == 0){
                        //Somebody disconnected, get his details and print
                        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                        DEBUG(dbgNet, "\n\tHost disconnected, ip " << inet_ntoa(address.sin_addr) << " , port " << ntohs(address.sin_port));
                        //Close the socket and mark as 0 in list for reuse
                        unixClose(sd);
                        client_socket[i] = -1;
                    }else{
                        printf("\n\t%s\n", buffer);

                        for(int i = 0; i<valread; i++){
                            buffer[i] = toupper(buffer[i]);
                        }

                        unixWrite(sd, buffer, strlen(buffer));
                    }
                    

                }
            }
        }

        return 0;
    }

    int init(){
        opt = true;

        memset(&address, 0, sizeof(address));

        //set all to empty slots
        for(int i = 0; i < MAX_CLIENTS; i++){
            client_socket[i] = -1;
        }

        //create our server socket
        if((ss_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
             DEBUG(dbgNet, "\n\tServerSocket::init() - socket failed");
            return error_code = -1;
        }

        //copy from geekforgeeks
        //set master socket to allow multiple connections ,  
        //this is just a good habit, it will work without this  
        if( setsockopt(ss_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )   
        {   
            DEBUG(dbgNet, "\n\tServerSocket::init() - setsockopt");   
            return error_code =-2;
        }

        //create a IPv4 server socket
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        //bind our socket
        if(bind(ss_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
            DEBUG(dbgNet, "\n\tServerSocket::init() - bind failed");
            return error_code =-3;
        }

        DEBUG(dbgNet, "\n\tServerSocket::init() - bind done");

        //set up our server can listen up to 3 pending connections
        if(unixListen(ss_fd, 3) < 0){
            DEBUG(dbgNet, "\n\tServerSocket::init() - listen failed");
            return error_code =-4;
        }

        //calculate address length
        addrlen = sizeof(address);        
        return 0;
    }























        
    int open() override{
        return 0;
    }

    int read(char* buffer, int size) override{
        return 0;
    };

    int write(char* buffer, int size) override{
        return 0;
    };
    int seek(int pos) override{return -1;};
    int close() override{
        return 0;
    };
    bool canRead() override{return true;};
    bool canWrite() override{return true;};
    int length() override{return -1;};
    void getName(char* buffer, int size){
        strncpy(buffer, ip_address, size);
    };



    ~ServerSocket(){
        if(ss_fd != -1){
            close();
        }
        
    };   

};

int runTestSocket(int port){
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[30] ,  
          max_clients = 30 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   
         
    char buffer[1025];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
         
    //a message  
    char *message = "ECHO Daemon v1.0 \r\n";   
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( port );   
         
    //bind the socket to localhost port 8888  
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port %d \n", port);   
         
    //try to specify maximum of 3 pending connections for the master socket  
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ...");   
         
    while(TRUE)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
                  (address.sin_port));   
           
            //send new connection greeting message  
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            }   
                 
            puts("Welcome message sent successfully");   
                 
            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address , \ 
                        (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));   
                         
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';   
                    send(sd , buffer , strlen(buffer) , 0 );   
                }   
            }   
        }   
    }
    return 0;
}

#endif // _SERVERSOCKET_H