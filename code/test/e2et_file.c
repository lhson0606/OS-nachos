#include "syscall.h"
#include "copyright.h"

#define MaxBufferSize 1024
#define ConsoleInput 0
#define ConsoleOutput 1

int main(){
    /******/
    int create_result;
    int open_result;
    int write_result;
    int read_result;
    int close_result;
    int remove_result;
    int saved_fd;
    char* random_content = "Hello world";
    char read_buffer[MaxBufferSize];
    /*****/

    //*****************************************************************************************//
    //test Create()
    Write("\n\n****File creation test****\n", MaxBufferSize, ConsoleOutput);
    create_result = Create("test.txt");
    if(create_result != -1)
        Write("\tCan create file: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCan create file: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //*****************************************************************************************//
    //test Open()
    open_result = Open("test.txt", 0);
    if(open_result != -1)
        Write("\tCan open existed file with mode RO: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCan open existed file with mode RO: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    Close(open_result);

    saved_fd = Open("test.txt", 1);
    if(saved_fd != -1)
        Write("\tCan open existed file with mode RW: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCan open existed file with mode RW: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);
    
    open_result = Open("test.txt", 0);
    if(open_result == -1)
        Write("\tCannot open file that already opened: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCannot open file that already opened: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);

    // Setup for the next test
    Close(saved_fd);
    Remove("test.txt");

    // Test Open() with a non-existent file
    open_result = Open("test.txt", 0);
    if(open_result == -1)
        Write("\tCannot open file that doesn't exist: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCannot open file that doesn't exist: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //*****************************************************************************************//
    //test Write()
    Write("\n\n****Write file test****\n", MaxBufferSize, ConsoleOutput);
    //setup for the next test
    Create("test.txt");
    open_result = Open("test.txt", 1);
    write_result = Write(random_content, 11, open_result);

    if(write_result == 11)
        Write("\tCan write to file that open in mode RW: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\Can write to file that open in mode RW: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);
    
    //setup for the next test
    Close(open_result);
    open_result = Open("test.txt", 0);
    write_result = Write(random_content, 11, open_result);

    if(write_result == -1)
        Write("\tCannot write to file that opened in mode RO: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCannot write to file that opened in mode RO: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //*****************************************************************************************//
    //test Read()
    Write("\n\n****Read file test****\n", MaxBufferSize, ConsoleOutput);
    //setup for the next test
    Close(open_result);
    Remove("test.txt");
    Create("test.txt");
    open_result = Open("test.txt", 1);
    Write(random_content, 11, open_result);
    Close(open_result);
    
    open_result = Open("test.txt", 0);
    read_result = Read(read_buffer, 11, open_result);

    if(read_result == 11)
        Write("\tCan read from file that opened in mode RO: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCan read from file that opened in mode RO: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //setup for the next test
    Close(open_result);
    read_result = Read(read_buffer, 11, open_result);

    if(read_result == -1)
        Write("\tCannot read from file that is not opened: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCannot read from file that is not opened: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);
    //clena up
    Close(open_result);
    Remove("test.txt");
    
    //*****************************************************************************************//
    //test Close()
    Write("\n\n****Close file test****\n", MaxBufferSize, ConsoleOutput);
    //setup for the next test
    Create("test.txt");
    open_result = Open("test.txt", 1);
    close_result = Close(open_result);

    if(close_result == 0)
        Write("\tCan close file with valid file descriptor: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCan close file with valid file descriptor: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //setup for the next test
    close_result = Close(-1);

    if(close_result == -1)
        Write("\tCannot close file with invalid file descriptor: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCannot close file with invalid file descriptor: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //clean up
    Close(open_result);
    Remove("test.txt");

    //*****************************************************************************************//
    //test Remove()
    Write("\n\n****Remove file test****\n", MaxBufferSize, ConsoleOutput);
    //setup for the next test
    Create("test.txt");
    open_result = Open("test.txt", 1);
    close_result = Close(open_result);

    remove_result = Remove("test.txt");

    if(remove_result == 0)
        Write("\tCan remove file that is not opened: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCan remove file that is not opened: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //setup for the next test
    Create("test.txt");
    open_result = Open("test.txt", 1);
    remove_result = Remove("test.txt");

    if(remove_result == -1)
        Write("\tCannot remove file that is opened: \r\t\t\t\t\t\t\t\t\t\tpassed\n", MaxBufferSize, ConsoleOutput);
    else
        Write("\tCannot remove file that is opened: \r\t\t\t\t\t\t\t\t\t\tfailed\n", MaxBufferSize, ConsoleOutput);

    //clean up
    Remove("test.txt");

    Halt();
}
