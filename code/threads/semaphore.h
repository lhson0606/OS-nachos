//
// Created by khuong on 12/7/23.
//

class Semaphore {
private:
    char name[50];
    Semaphore* semaphore;

public:
    Semaphore(char* na,int i);
    ~Semaphore();
    void Wait();
    void Signal();
    char* GetName();
};