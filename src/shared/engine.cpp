#include "../engine/engine.hpp"
#include "../engine/mlib/mlib.hpp"
#include "variable.hpp"

void Engine::post_constructor()
{

}

void Engine::pre_init()
{
    initDungeonData();
}

void Engine::post_init()
{
    Out = "Loading first map...\n";
    setMap("mapStartup");
}

void Engine::pre_update()
{

}

void Engine::post_update()
{

}
