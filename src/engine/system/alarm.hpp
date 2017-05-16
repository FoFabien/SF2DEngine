#ifndef ALARM_HPP
#define ALARM_HPP
#define _USE_ALARM_
#ifdef _USE_ALARM_

#include <SFML/System/Clock.hpp>
#include <stdint.h>

class Alarm
{
    public:
        Alarm(int32_t id, const sf::Time &target);
        ~Alarm();
        // implemented in "game_alarm.cpp"
        bool update(); /// update the alarm timer
    protected:
        void run(); /// run custom code
        bool running; /// start at true, become false if the target time is reached
        int32_t id; /// id
        sf::Time current; /// current time elapsed
        sf::Time target; /// target time
};

#include "../../shared/alarm.hpp"

#endif
#endif // ALARM_HPP
