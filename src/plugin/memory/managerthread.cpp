#include "managerthread.hpp"
#include "../../mlib/mlib.hpp"

#include <SFML/System/Mutex.hpp>
extern sf::Mutex mutex;

ManagerThread::ManagerThread()
{
    //ctor
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
            Out = "Thread: Waiting for thread (0x" + mlib::int2hex((int32_t)i->ptr) + ")...\n";
            mutex.unlock();
            i->ptr->wait();
        }
        else mutex.unlock();
        delete i->ptr;
        delete i;
    }
    threads.clear();
}

void ManagerThread::run(int32_t id)
{
    ThreadArguments ta;
    run(id, ta);
}

void ManagerThread::run(int32_t id, int32_t i, std::string s)
{
    ThreadArguments ta;
    ta.iArgs.push_back(i);
    ta.sArgs.push_back(s);
    run(id, ta);
}

void ManagerThread::run(int32_t id, std::vector<int32_t> iArgs, std::vector<std::string> sArgs)
{
    ThreadArguments ta;
    ta.iArgs = iArgs;
    ta.sArgs = sArgs;
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
