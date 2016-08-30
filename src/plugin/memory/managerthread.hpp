#ifndef MANAGERTHREAD_HPP
#define MANAGERTHREAD_HPP

#include <SFML/System/Thread.hpp>
#include <vector>
#include <string>
#include <stdint.h>

struct ThreadContainer
{
    sf::Thread* ptr;
    bool running;
};

struct ThreadArguments
{
    std::vector<int32_t> iArgs;
    std::vector<std::string> sArgs;
    ThreadContainer* itself;
};

class ManagerThread
{
    public:
        ManagerThread();
        ~ManagerThread();
        void clear();
        void run(int32_t id);
        void run(int32_t id, int32_t i, std::string s);
        void run(int32_t id, std::vector<int32_t> iArgs, std::vector<std::string> sArgs);
        void run(int32_t id, ThreadArguments args); // implemented in game_thread.cpp
        void garbageCollection();
    protected:
        std::vector<ThreadContainer*> threads;
};

#include "../../game/thread.hpp"

#endif // MANAGERTHREAD_HPP
