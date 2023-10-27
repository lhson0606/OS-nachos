#include "openstream.h"
#include "network.h"

class Socket : public OpenStream
{
public:
    Socket(){};

    int open() override{return -1;};
    int read(char* buffer, int size) override{return -1;};
    int write(char* buffer, int size) override{return -1;};
    int seek(int pos) override{return -1;};
    int close() override{return -1;};
    bool canRead() override{return true;};
    bool canWrite() override{return true;};
    int length() override{return -1;};
    void getName(char* buffer, int size){
        strncpy(buffer, ip_address, size);
    };

    ~Socket(){
        delete is;
        delete os;
    };
private:
    int socket;
    int pos;
    char* ip_address;
    int port;
    int type;
    int protocol;
    int status;
    NetworkAddress* addr;
    NetworkInput* is;
    NetworkOutput* os;
};