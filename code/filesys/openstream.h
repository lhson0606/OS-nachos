#ifndef _OPENSTREAM_H_
#define _OPENSTREAM_H_

#define MAX_FILE_NAME_LENGTH 32
#define MAX_ALLOWED_BUFFER_SIZE 64

class OpenStream
{
public:
    OpenStream() {};
    virtual ~OpenStream() {};
    virtual int open() =0;
    virtual int read(char* buffer, int size)=0;
    virtual int write(char* buffer, int size)=0;
    virtual int seek(int pos) =0;
    virtual int close() =0;
    virtual int length() =0;
    virtual bool canRead() =0;
    virtual bool canWrite() =0;
    virtual void getName(char* buffer, int size) =0;
protected:
    int pos;
};


#endif // _OPENSTREAM_H_