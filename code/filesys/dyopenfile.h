
#ifndef _DYOPENFILE_H_
#define _DYOPENFILE_H_

#include "openstream.h"
#include "openfile.h"
#include "main.h"

// Our own definitions
typedef int OpenMode;
typedef int OpenFileID;
typedef char FileName[MAX_FILE_NAME_LENGTH];
static const OpenMode NO_MODE = -1;
static const OpenFileID STDIN = 0;
static const OpenFileID STDOUT = 1;
static const OpenMode RO = 0;
static const OpenMode RW = 1;

class DyOpenFile : public OpenStream
{
public:
    DyOpenFile(FileName name, OpenMode mode){
        //file_name = new char[MAX_FILE_NAME_LENGTH];
        strncpy(file_name, name, MAX_FILE_NAME_LENGTH);

        openfile = NULL;
        read_pos = 0;
        write_pos = 0;
        open_mode = mode;
    }

    virtual ~DyOpenFile(){
        close();
    }

    int open() override{
        DEBUG(dbgFile, "\n\tOpening file " << file_name << " with mode " << open_mode);
        this->openfile = kernel->fileSystem->Open(file_name);

        if(!openfile){
            DEBUG(dbgFile, "\n\tclass DyOpenFile: file not found or file currently cannot be opened " << file_name);
            return -1;
        }
        
        DEBUG(dbgFile, "\n\tclass DyOpenFile: file opened " << file_name);
        return 0;
    }

    int read(char* buffer, int size) override{
        int read_size = openfile->ReadAt(buffer, size, read_pos);

        if(read_size == -1){
            return -1;
        }

        read_pos += read_size;
        return read_size;
    }

    int write(char* buffer, int size) override{
        if(open_mode == RO){
            return -1;
        }
        DEBUG(dbgFile,"Buffer"<<buffer <<" "<<size );
        int write_size = openfile->WriteAt(buffer, size, write_pos);

        if(write_size == -1){
            return -1;
        }

        write_pos += write_size;
        return write_size;
    }

    int seek(int pos) override{
        int res = openfile->Seek(pos);

        if(res == -1){
            return -1;
        }

        read_pos = pos;
        return read_pos;
    }

    int close() override{
        delete openfile;
        //desconstructors are called automatically, prevent double delete
        openfile = NULL;
    }

    bool canRead() override{
        return open_mode != NO_MODE;
    }

    bool canWrite() override{
        return open_mode == RW;
    }

    int length() override{
        return openfile->Length();
    }

    void getName(char* buffer, int size){
        strncpy(buffer, file_name, size);
    }

private:
    FileName file_name;
    OpenFile *openfile;
    int read_pos;
    int write_pos;
    int open_mode;
};


#endif // _DYOPENFILE_H_