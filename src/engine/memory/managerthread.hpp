#ifndef MANAGERTHREAD_HPP
#define MANAGERTHREAD_HPP

#include <SFML/System/Mutex.hpp>
#include <SFML/System/Thread.hpp>
#include <vector>
#include <string>
#include <stdint.h>

struct ThreadContainer
{
    sf::Thread* ptr;
    bool running;
    int32_t id;
};

struct ThreadArguments
{
    std::vector<int32_t> iArgs;
    std::vector<std::string> sArgs;
    ThreadContainer* itself = nullptr;
    sf::Mutex* mutex = nullptr;
};

class ManagerThread
{
    public:
        ManagerThread();
        ~ManagerThread();
        void clear();
        void run(int32_t id, sf::Mutex *m = nullptr);
        void run(int32_t id, int32_t i, std::string s, sf::Mutex *m = nullptr);
        void run(int32_t id, std::vector<int32_t> iArgs, std::vector<std::string> sArgs, sf::Mutex *m = nullptr);
        void run(int32_t id, ThreadArguments args); // implemented in game_thread.cpp
        void garbageCollection();
        bool sameIdExist(const ThreadContainer* ptr);
    protected:
        sf::Mutex mutex;
        std::vector<ThreadContainer*> threads;
};

#include "../../shared/thread.hpp"

#endif // MANAGERTHREAD_HPP
