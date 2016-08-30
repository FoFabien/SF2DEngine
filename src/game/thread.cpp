#include "../plugin/memory/managerthread.hpp"
#include "../engine.hpp"
#include "../mlib/mlib.hpp"

#include <SFML/System/Mutex.hpp>
extern sf::Mutex mutex;

void ManagerThread::run(int32_t id, ThreadArguments args)
{
    ThreadContainer* tc = new ThreadContainer();
    args.itself = tc;

    switch(id)
    {
        case 0: tc->ptr = new sf::Thread(t_testrun, args); break;
        case 1: tc->ptr = new sf::Thread(t_loadmap, args); break;
        case 2: tc->ptr = new sf::Thread(t_createfilesystem, args); break;
        case 3: tc->ptr = new sf::Thread(t_runServer, args); break;
        case 4: tc->ptr = new sf::Thread(t_runClient, args); break;
        default:
            delete tc;
            return;
    }

    tc->running = true;
    tc->ptr->launch();
    threads.push_back(tc);

    mutex.lock();
    Out = "Thread ID(" + mlib::int2str(id) + ") launched\n";
    mutex.unlock();
}

void t_testrun(ThreadArguments args)
{
    for(uint32_t i = 0; i < 1000000 && engine.isRunning(); ++i)
    {
        mutex.lock();
        if(i % 10000 == 0) Out = "this is a thread\n";
        mutex.unlock();
    }
    mutex.lock();
    args.itself->running = false;
    mutex.unlock();
}

void t_loadmap(ThreadArguments args)
{
    #ifdef _USE_MAP_
    if(!args.iArgs.empty() && !args.sArgs.empty() && engine.isRunning())
    {
        mutex.lock();
        Out = "Thread: loading map \"" + args.sArgs[0] + "\"...\n";
        switch(args.iArgs[0])
        {
            default: engine.mapspace = new Map(); break;
            case 1: engine.mapspace = new DungeonMap(); break;
        }
        mutex.unlock();

        if(engine.mapspace->loadFromMemory(args.sArgs[0]))
        {
            mutex.lock();
            Out = "Thread: map \"" + args.sArgs[0] + "\" loaded\n";
            engine.mapspace->setLoadedFlag();
            mutex.unlock();
        }
    }
    #endif

    mutex.lock();
    args.itself->running = false;
    mutex.unlock();
}

void t_createfilesystem(ThreadArguments args)
{
    mutex.lock();
    #ifdef _USE_FILESYSTEM_
    if(!args.iArgs.empty() && !args.sArgs.empty())
    {
        if(!engine.files.create("data.pack", args.iArgs[0], args.sArgs[0]))
            Out = "Thread: \"data.pack\" isn't found or valid\n";
    }
    #endif
    args.itself->running = false;
    mutex.unlock();
}

void t_runServer(ThreadArguments args)
{
    #ifdef _USE_NETWORK_
    while(engine.isRunning())
    {
        if(engine.server.isRunning())
        {
            engine.server.update();
            engine.server.receive();
            std::vector<uint8_t> ids = engine.server.getConnectedID();
            for(uint8_t &i : ids)
            {
                if(engine.server.hasDataFromClient(i))
                {
                    sf::Packet p;
                    p = engine.server.getPacketFromClient(i);
                    //
                    // do stuff
                    //
                }
            }
        }
        else sf::sleep(sf::milliseconds(100));
    }
    #endif

    mutex.lock();
    args.itself->running = false;
    mutex.unlock();
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

    mutex.lock();
    args.itself->running = false;
    mutex.unlock();
}
