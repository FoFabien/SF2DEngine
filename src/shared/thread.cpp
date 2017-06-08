#include "../engine/memory/managerthread.hpp"
#include "../engine/engine.hpp"
#include "../engine/mlib/mlib.hpp"
#include <SFML/System/Sleep.hpp>

void ManagerThread::run(int32_t id, ThreadArguments args)
{
    sf::Lock lock(mutex);
    ThreadContainer* tc = new ThreadContainer();
    args.itself = tc;
    if(!args.mutex)
        args.mutex = &mutex;
    tc->id = id;
    switch(id)
    {
        case 0: tc->ptr = new sf::Thread(t_testrun, args); break;
        case 1: tc->ptr = new sf::Thread(t_loadmap, args); break;
        case 2: tc->ptr = new sf::Thread(t_createfilesystem, args); break;
        case 3: tc->ptr = new sf::Thread(t_runServer, args); break;
        case 4: tc->ptr = new sf::Thread(t_runClient, args); break;
        default: delete tc; return;
    }

    tc->running = true;
    tc->ptr->launch();
    threads.push_back(tc);

    Out = "Thread (ID:" + mlib::int2str(id) + ") launched\n";
}

void t_testrun(ThreadArguments args)
{
    for(uint32_t i = 0; i < 1000000 && engine.isRunning(); ++i)
    {
        engine.mutexLock();
        if(i % 10000 == 0) Out = "this is a thread\n";
        engine.mutexUnlock();
    }
    args.mutex->lock();
    args.itself->running = false;
    args.mutex->unlock();
}

void t_loadmap(ThreadArguments args)
{
    #ifdef _USE_MAP_

    if(!engine.threads.sameIdExist(args.itself))
    {
        while(engine.isRunning())
        {
            args.mutex->lock();
            if(engine.change_map && !engine.mapspace)
            {
                engine.mutexLock();
                Out = "Thread: loading map \"" + engine.next_map + "\"...\n";
                std::string buffer = engine.next_map;
                switch(engine.map_type)
                {
                    default: engine.mapspace = new Map(); break;
                    case 1: engine.mapspace = new DungeonMap(); break;
                }
                engine.mutexUnlock();

                if(engine.mapspace->loadFromMemory(buffer))
                {
                    engine.mutexLock();
                    Out = "Thread: map \"" + buffer + "\" loaded\n";
                    engine.mapspace->setLoadedFlag();
                    engine.mutexUnlock();
                }
            }
            else
            {
                sf::sleep(sf::milliseconds(30));
            }
            args.mutex->unlock();
        }
    }
    #endif

    args.mutex->lock();
    args.itself->running = false;
    args.mutex->unlock();
}

void t_createfilesystem(ThreadArguments args)
{
    #ifdef _USE_FILESYSTEM_
    args.mutex->lock();
    if(!args.iArgs.empty() && !args.sArgs.empty())
    {
        if(!engine.files.create("data.pack", args.iArgs[0], args.sArgs[0]))
        {
            engine.mutexLock();
            Out = "Thread: \"data.pack\" isn't found or valid\n";
            engine.mutexUnlock();
        }
    }
    args.mutex->unlock();
    #endif
    args.mutex->lock();
    args.itself->running = false;
    args.mutex->unlock();
}

void t_runServer(ThreadArguments args)
{
    #ifdef _USE_NETWORK_
    while(engine.isRunning())
    {

    }
    #endif

    args.mutex->lock();
    args.itself->running = false;
    args.mutex->unlock();
}

void t_runClient(ThreadArguments args)
{
    #ifdef _USE_NETWORK_
    while(engine.isRunning())
    {

            //
            // do stuff
            //

    }
    #endif

    args.mutex->lock();
    args.itself->running = false;
    args.mutex->unlock();
}
