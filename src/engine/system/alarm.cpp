#include "alarm.hpp"

#ifdef _USE_ALARM_
#include "../engine.hpp"

Alarm::Alarm(int32_t id, const sf::Time &target) : running(true), id(id), current(sf::Time::Zero), target(target) {}

Alarm::~Alarm() {}

bool Alarm::update()
{
    if(!running) return false;
    current += engine.getElapsedTickTime(); // add the elapsed time
    if(current >= target) // compare with the targeted time
    {
        run(); // run code if target is reached
        running = false;
        return false;
    }
    return true;
}
#endif
