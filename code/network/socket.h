#include "openstream.h"
#include "network.h"

class Socket : public OpenStream
{
public:
    Socket(){
        
    };

    int open() override{
        socket = OpenSocket();

        if (socket == -1){
            return -1;
        }

        DEBUG(dbgNet, "Socket opened "<< socket);

        return 0;
    }

    int read(char* buffer, int size) override{        
        input->Receive(buffer);
        DEBUG(dbgNet, "Socket read "<< buffer );
        return 0;
    };

    int write(char* buffer, int size) override{
        PacketHeader ph;
        //ph.from = 0;
        ph.to = 0;
        ph.length = size;
        isSending = true;
        // wait for previous send to finish
        output->Send(ph, buffer);
        
        //input->Receive(buffer);
        //DEBUG(dbgNet, "Socket wrote "<< buffer );

    };
    int seek(int pos) override{return -1;};
    int close() override{
        CloseSocket(socket);
        socket = -1;
        return 0;
    };
    bool canRead() override{return true;};
    bool canWrite() override{return true;};
    int length() override{return -1;};
    void getName(char* buffer, int size){
        strncpy(buffer, ip_address, size);
    };

    int connect(char* ip, int port){
        DEBUG(dbgNet, "Connecting to " << ip << ":" << port);
        strncpy(ip_address, ip, MAX_FILE_NAME_LENGTH);
        this->port = port;
        
        input = new NetworkInput(receivedCallbackObj);
        output = new NetworkOutput(1.0, sentCallbackObj);

        return 0;
    }

    class NetworkInputCallback : public CallBackObj{
    public:
        NetworkInputCallback(Socket* socket){
            this->socket = socket;
        };

        void CallBack(){
            socket->receivedCallback();
        };
    private:
        Socket* socket;
    };

    class NetworkOutputCallback : public CallBackObj{
    public:
        NetworkOutputCallback(Socket* socket){
            this->socket = socket;
        };

        void CallBack(){
            socket->sentCallback();
        };
    private:
        Socket* socket;
    };

    ~Socket(){
        // close socket if it is open
        if(socket != -1){
            close();
        }
        delete receivedCallbackObj;
        delete sentCallbackObj;
    };
private:
    void receivedCallback(){
        DEBUG(dbgNet, "Received callback");
    }

    void sentCallback(){
        DEBUG(dbgNet, "Sent callback");
        isSending = false;
    }

    bool isSending = false;

    int socket;
    int pos;
    FileName ip_address;
    int port;
    int type;
    int protocol;
    int status;
    NetworkInput* input;
    NetworkOutput* output;
    NetworkInputCallback* receivedCallbackObj;
    NetworkOutputCallback* sentCallbackObj;
    int const MaxMailSize = 1024;
};