#ifndef _FDT_H_
#define _FDT_H_

#include "dyopenfile.h"
#include "socket.h"
#include "debug.h"
#include "hash.h"

#define MAX_FILE_DESCRIPTOR 20

class FileDescriptorTable
{
private:
    //store the file descriptors
    OpenStream **fdt;

    /**
     * @brief      Gets the free file descriptors.
     * @return     The free file descriptor on success, -1 on fail (is full).
     */
    OpenFileID getFreeFileDescriptor();

    static FileDescriptorTable* instance;
public:
    FileDescriptorTable();

    /**
     * @brief      Adds a stream.
     * @param      stream  The stream
     * @return     The file descriptor on success, -1 on fail (is full).
    */
    int Add(OpenStream *stream);

    /**
     * @brief      Gets the file.
     * @param[in]  fd    The file descriptor
     * @return     The OpenStream on success, NULL on fail.
     */
    OpenStream *getStream(OpenFileID fd);

    /**
     * @brief      Determines if has free file descriptor.
     * @return     True if has free file descriptor, False otherwise.
    */
    bool hasFreeFileDescriptor();

    ~FileDescriptorTable();
    
    /**
     * @brief      Determines if open.
     * @param      name  The name
     * @return     True if open, False otherwise.
    */
    bool isOpen(char* name);

    /**
     * @brief      Determines if open.
     * @param[in]  fd    The file descriptor
     * @return     True if open, False otherwise.
    */
    bool isOpen(OpenFileID fd);

    /**
     * @brief      Close a stream given a file descriptor
     * @param[in]  fd    The file descriptor
     * @return     0 on success, -1 if an error occured
    */
    int close(OpenFileID fd);

    static FileDescriptorTable* getInstance();

};
#endif // _FDT_H_

