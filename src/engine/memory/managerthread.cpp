#include "managerthread.hpp"
#include "../mlib/mlib.hpp"
#include <SFML/System/Lock.hpp>

ManagerThread::ManagerThread()
{
    threads.reserve(10);
}

ManagerThread::~ManagerThread()
{
    clear();
}

void ManagerThread::clear()
{
    for(ThreadContainer* &i: threads)
    {
        mutex.lock();
        if(i->running)
        {
            Out = "Thread: Waiting for thread (ID:" + std::to_string(i->id) + ")...\n";
            mutex.unlock();
            i->ptr->wait();
        }
        else mutex.unlock();
        delete i->ptr;
        delete i;
    }
    threads.clear();
}

void ManagerThread::run(int32_t id, sf::Mutex *m)
{
    ThreadArguments ta;
    ta.mutex = m;
    run(id, ta);
}

void ManagerThread::run(int32_t id, int32_t i, std::string s, sf::Mutex *m)
{
    ThreadArguments ta;
    ta.iArgs.push_back(i);
    ta.sArgs.push_back(s);
    ta.mutex = m;
    run(id, ta);
}

void ManagerThread::run(int32_t id, std::vector<int32_t> iArgs, std::vector<std::string> sArgs, sf::Mutex *m)
{
    ThreadArguments ta;
    ta.iArgs = iArgs;
    ta.sArgs = sArgs;
    ta.mutex = m;
    run(id, ta);
}

void ManagerThread::garbageCollection()
{
    for(size_t i = 0; i < threads.size(); ++i)
    {
        mutex.lock();
        if(!threads[i]->running)
        {
            delete threads[i]->ptr;
            delete threads[i];
            threads.erase(threads.begin()+i);
            i--;
        }
        mutex.unlock();
    }
}

bool ManagerThread::sameIdExist(const ThreadContainer* ptr)
{
    sf::Lock lock(mutex);
    for(ThreadContainer* i : threads)
        if(i->id == ptr->id && i != ptr) return true;
    return false;
}
