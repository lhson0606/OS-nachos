#include "fdt.h"

FileDescriptorTable* FileDescriptorTable::instance = NULL;

FileDescriptorTable::FileDescriptorTable()
{
    fdt = new OpenStream *[MAX_FILE_DESCRIPTOR]
    { NULL };
}

int FileDescriptorTable::Add(OpenStream *stream)
{
    int fd = getFreeFileDescriptor();
    if (fd == -1)
    {
        return -1;
    }

    fdt[fd] = stream;
    return fd;
}

/**
 * @brief      Gets the file.
 * @param[in]  fd    The file descriptor
 * @return     The OpenStream on success, NULL on fail.
 */
OpenStream *FileDescriptorTable::getStream(OpenFileID fd)
{
    if (fd < 0 || fd >= MAX_FILE_DESCRIPTOR || fd == 0 || fd == 1)
    {
        DEBUG(dbgFile, "\n\tInvalid file descriptor " << fd);
        return NULL;
    }
    OpenStream *open_stream = fdt[fd];
    return open_stream;
}

bool FileDescriptorTable::FileDescriptorTable::hasFreeFileDescriptor()
{
    return getFreeFileDescriptor() != -1;
}

FileDescriptorTable::~FileDescriptorTable()
{
    for (int i = 0; i < MAX_FILE_DESCRIPTOR; i++)
    {
        // resume that if file is closed then names[i] is NULL
        delete fdt[i];
    };
    delete[] fdt;
}

bool FileDescriptorTable::isOpen(char *name)
{
    FileName current_name;
    // start from 2 because 0 and 1 are reserved for stdin and stdout
    for (int i = 2; i < MAX_FILE_DESCRIPTOR; i++)
    {
        if (!fdt[i])
            continue;

        fdt[i]->getName(current_name, MAX_FILE_NAME_LENGTH);
        if (strcmp(current_name, name) == 0)
            return isOpen(i);
    }
    return false;
}

bool FileDescriptorTable::isOpen(OpenFileID fd)
{
    return fdt[fd] != NULL;
}

int FileDescriptorTable::close(OpenFileID fd)
{
    if (fd < 0 || fd >= MAX_FILE_DESCRIPTOR || fd == 0 || fd == 1)
    {
        DEBUG(dbgFile, "\n\tInvalid file descriptor " << fd);
        return -1;
    }

    if (!fdt[fd])
    {
        DEBUG(dbgFile, "\n\tFile descriptor " << fd << " is not open");
        return -1;
    }

    delete fdt[fd];
    fdt[fd] = NULL;
    return 0;
}

OpenFileID FileDescriptorTable::getFreeFileDescriptor()
{
    OpenFileID fd;
    fd = -1;
    // we start from 2 because 0 and 1 are reserved for stdin and stdout
    for (int i = 2; i < MAX_FILE_DESCRIPTOR; i++)
    {
        if (!fdt[i])
        {
            fd = i;
            break;
        }
    }

    return fd;
}

FileDescriptorTable* FileDescriptorTable::getInstance(){
    if(instance == NULL){
        instance = new FileDescriptorTable();
    }
    return instance;
}