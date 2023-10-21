#ifndef _FDT_H_
#define _FDT_H_

// Our own definitions
typedef int OpenMode;
typedef int OpenFileID;
typedef char *FileName;
// End of our own definitions
const int MAX_FILE_DESCRIPTOR = 20;
const OpenMode NO_MODE = -1;
const OpenFileID STDIN = 0;
const OpenFileID STDOUT = 1;
const OpenMode RO = 0;
const OpenMode RW = 1;
#include "openfile.h"
#include "debug.h"

class FileDescriptorTable
{
public:
    FileDescriptorTable();

    /**
     * @brief      Opens a file.
     * @param[in]  name  The name of the file
     * @param[in]  mode  The mode of the file [RO, RW]
     * @return     The file descriptor on success, -1 on fail.
    */
    OpenFileID Open(FileName name, OpenMode mode);

    /**
     * @brief      Closes a file.
     * @param[in]  fd    The file descriptor
     * @return     0 on success, -1 on fail.
     */
    int Close(OpenFileID fd);

    /**
     * @brief      Counts the file descriptors.
     * @return     The number of file descriptor (>=2) reserved for stdin, stdout.
     */
    int countFileDescriptor();

    /**
     * @brief      Gets the file.
     * @param[in]  fd    The file descriptor
     * @return     The file on success, NULL on fail.
     */
    OpenFile *getFile(OpenFileID fd);

    /**
     * @brief      Check if the file descriptor is read only.
     * @param[in]  fd    The file descriptor
     * @return     True if read only, False otherwise.
    */
    bool isReadOnly(OpenFileID fd);

    ~FileDescriptorTable();

private:
    //store the file descriptors
    OpenFile **fdt;

    /**
     * store the number of file descriptors
     * NO_MODE = -1 means that the table is not initialized
     * RO = 0       means can read only
     * RW = 1       means can read and write
     */
    OpenMode *modes;  

    /**
     * @brief      Gets the free file descriptors.
     * @return     The free file descriptor on success, -1 on fail (is full).
     */
    OpenFileID getFreeFileDescriptor();
};

#endif // _FDT_H_