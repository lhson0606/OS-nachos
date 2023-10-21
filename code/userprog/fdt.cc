#include "main.h"
#include "fdt.h"

FileDescriptorTable::FileDescriptorTable()
{
    fdt = new OpenFile *[MAX_FILE_DESCRIPTOR]{ NULL };
    modes = new OpenMode[MAX_FILE_DESCRIPTOR]{ NO_MODE };
    modes[STDIN] = RW;
    modes[STDOUT] = RW;
}

OpenFileID FileDescriptorTable::Open(FileName filename, OpenMode mode)
{
    DEBUG(dbgFile, "\n\tOpening file " << filename << " with mode " << mode);
    OpenFile *file = kernel->fileSystem->Open(filename);

    if (!file)
    {
        DEBUG(dbgFile, "\n\tFile not found or file currently cannot be opened " << filename);
        return -1;
    }
    else
    {
        OpenFileID id = getFreeFileDescriptor();

        if (id == -1)
        {
            DEBUG(dbgFile, "\n\tNo free file descriptors");
            return -1;
        }
        else
        {
            DEBUG(dbgFile, "\n\tFile opened with id " << id);
            fdt[id] = file;
            modes[id] = mode;
            return id;
        }
    }
}

int FileDescriptorTable::Close(OpenFileID id)
{
    DEBUG(dbgFile, "\n\tClosing file descriptor " << id)
    if (id < 2 || id >= MAX_FILE_DESCRIPTOR)
    {
        DEBUG(dbgFile, "\n\tInvalid file descriptor " << id);
        return -1;
    }
    else
    {
        OpenFile *file;
        file = getFile(id);
        if (file == NULL)
        {
            DEBUG(dbgFile, "\n\tFile descriptor " << id << " is not open");
            return -1;
        }
        else
        {
            file->~OpenFile();
            DEBUG(dbgFile, "\n\tFile descriptor " << id << " closed");
            fdt[id] = NULL;
            modes[id] = NO_MODE;
            return 0;
        }
    }
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

int FileDescriptorTable::countFileDescriptor()
{
    int count;
    count = 0;
    for (int i = 2; i < MAX_FILE_DESCRIPTOR; i++)
    {
        if (fdt[i])
        {
            count++;
        }
    }
    return count;
}

OpenFile* FileDescriptorTable::getFile(OpenFileID fd)
{
    if (fd < 0 || fd >= MAX_FILE_DESCRIPTOR || fd == 0 || fd == 1)
    {
        DEBUG(dbgFile, "\n\tInvalid file descriptor " << fd);
        return NULL;
    }
    OpenFile *file = fdt[fd];
    return file;
}

bool FileDescriptorTable::isReadOnly(OpenFileID fd){
    return modes[fd] == RO;
}

FileDescriptorTable::~FileDescriptorTable()
{
    delete[] fdt;
    delete[] modes;
}
