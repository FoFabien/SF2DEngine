#ifndef GAME_THREAD_HPP
#define GAME_THREAD_HPP

// never include "game/thread.hpp" !! include "managerthread.hpp" instead

void t_testrun(ThreadArguments args);
void t_loadmap(ThreadArguments args);
void t_createfilesystem(ThreadArguments args);
void t_runServer(ThreadArguments args);
void t_runClient(ThreadArguments args);

#endif // GAME_THREAD_HPP
