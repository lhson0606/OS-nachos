#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "openstream.h"
#include "openfile.h"
#include "main.h"

class Socket : public OpenStream
{
public:
    Socket();

    /**
     * @brief create a socket
     * @param socket fd (C socket fd not our fd)
    */
    Socket(int sock_fd);

    /**
     * @brief no used
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
     * @return -1
    */
    void getName(char* buffer, int size);
    /**
     * @brief connect to a server
     * @param IPv4 address
     * @param port
     * @return unix socket fd if success, -1 if error
    */
    int connect(char* ip, int port);
    /**
     * @brief destructor, close socket if it is opening
    */
    int getSocketFd();
    ~Socket();

private:

    int socket_fd;
    char ip_address[32];
    int port;
    int const MaxMailSize = MAX_ALLOWED_BUFFER_SIZE;

};

#endif // SOCKET_H