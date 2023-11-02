#include "fdt.h"
#include "serversocket.h"
#include "socket.h"

int ssListen(int socket, int count){
    return listen(socket, count);

}

int ssRead(int socket, char* buffer, int size){
    return read(socket, buffer, size);
}

int ssWrite(int socket, char* buffer, int size){
    return write(socket, buffer, size);
}

int ssClose(int socket){
    return close(socket);
}

ServerSocket::ServerSocket(int port) : OpenStream(){
        this->port = port;
};

int ServerSocket::listen(){
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
            int sd = client_openfileID[i];

            if(client_openfileID[i] != -1){
                Socket* client = (Socket*) FileDescriptorTable::getInstance()->getStream(client_openfileID[i]);
                sd = client->getSocketFd();
            }

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

            new_socket = accept(ss_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

            if(new_socket < 0){
                DEBUG(dbgNet, "\n\tServerSocket::run() - accept error");
                return error_code = -5;
            }

            handleClientNewConnection(new_socket);
            
        }
    

        //else its some IO operation on some other socket
        for(i = 0; i< MAX_CLIENTS; i++){
            if(client_openfileID[i] == -1){
                continue;
            }

            Socket* client = (Socket*) FileDescriptorTable::getInstance()->getStream(client_openfileID[i]);
            int sd = client->getSocketFd();

            if(FD_ISSET(sd, &readfds)){
                
                DEBUG(dbgNet, "\n\tServerSocket::run() - New I/O dectected on fd " << client_openfileID[i]);
                DEBUG(dbgNet, "\n\tServerSocket::run() - Client sock: " << client->getSocketFd());

                handleClientCommingMessage(i);           

            }
        }
    }

    return 0;
}

int ServerSocket::init(){
    opt = true;

    memset(&address, 0, sizeof(address));

    //set all to empty slots this will be used to reference client sockets in our file descriptor table
    for(int i = 0; i < MAX_CLIENTS; i++){
        client_openfileID[i] = -1;
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
    if(ssListen(ss_fd, 3) < 0){
        DEBUG(dbgNet, "\n\tServerSocket::init() - listen failed");
        return error_code =-4;
    }

    //calculate address length
    addrlen = sizeof(address);        
    return 0;
}
int ServerSocket::open(){
    return -1;
}

int ServerSocket::read(char* buffer, int size){
    return -1;
};

int ServerSocket::write(char* buffer, int size){
    return -1;
};

int ServerSocket::seek(int pos){
    return -1;
};

//destructor will call this
int ServerSocket::close(){
    //return ssClose(ss_fd);
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(client_openfileID[i] != -1){
            FileDescriptorTable::getInstance()->close(client_openfileID[i]);
        }
    }
    int res = ssClose(ss_fd);

    if(res == 0){
        ss_fd = -1;
    }

    return res;
};

bool ServerSocket::canRead(){
    return false;
};

bool ServerSocket::canWrite(){
    return false;
};

int ServerSocket::length(){
    return -1;
};

void ServerSocket::getName(char* buffer, int size){
    strncpy(buffer, ip_address, size);
};

//fdt close() will call this
ServerSocket::~ServerSocket(){
    if(ss_fd != -1){
        close();
    }
};   

int ServerSocket::handleClientNewConnection(int new_socket){
    DEBUG(dbgNet, "\n\tnew_socket: " << new_socket);            

    if(new_socket < 0){
        DEBUG(dbgNet, "\n\tServerSocket::run() - accept error");
        return error_code = -5;
    }else{
        DEBUG(dbgNet, "\n\tServerSocket::run() - New connection, socket fd is " << new_socket << " , ip is : " << inet_ntoa(address.sin_addr) << " , port : " << ntohs(address.sin_port));
    }

    
    //add new socket to array of sockets
    for(int i = 0; i < MAX_CLIENTS; i++){
        //if position is empty
        if(client_openfileID[i] < 0){
            Socket* client = new Socket(new_socket);
            int add_res;
            if((add_res = FileDescriptorTable::getInstance()->Add(client)) < 0){
                DEBUG(dbgNet, "\n\tServerSocket::run() - Cannot add client to file descriptor table");
                delete client;
                client = NULL;
            }else{
                client_openfileID[i] = add_res;
                DEBUG(dbgNet, "\n\tServerSocket::run() - Adding new connection to fdt with fd: " << add_res);
            }
            break;
        }
    }

}

int ServerSocket::handleClientCommingMessage(int index){
    char buffer [MaxMailSize];
    int valread = -1;
    Socket* client = NULL;
    
    client = (Socket*) FileDescriptorTable::getInstance()->getStream(client_openfileID[index]);
    memset(buffer, 0, MaxMailSize);    
    valread = client->read(buffer, MaxMailSize);

    if(valread == 0){
        //Somebody disconnected, get his details and print
        getpeername(client->getSocketFd(), (struct sockaddr*)&address, (socklen_t*)&addrlen);
        DEBUG(dbgNet, "\n\tHost disconnected, ip " << inet_ntoa(address.sin_addr) << " , port " << ntohs(address.sin_port));
        //Close the socket and mark as 0 in list for reuse
        FileDescriptorTable::getInstance()->close(client_openfileID[index]);
        client_openfileID[index] = -1;
    }else{
        printf("\n\t%s\n", buffer);

        for(int i = 0; i<valread; i++){
            buffer[i] = toupper(buffer[i]);
        }

        //ssWrite(client->getSocketFd(), buffer, strlen(buffer));
        client->write(buffer, strlen(buffer));
    }
}