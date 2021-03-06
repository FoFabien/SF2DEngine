#include "../engine/system/alarm.hpp"
#ifdef _USE_ALARM_
#ifdef DEBUG_BUILD
#include "../engine/mlib/mlib.hpp"
#endif

void Alarm::run()
{
    #ifdef DEBUG_BUILD
    Out = "Alarm (" + std::to_string(id) + ") triggered (" + mlib::long2str(current.asMicroseconds()) + " microsecond(s))\n";
    #endif

    switch(id) // run custom code for each different alarm ID
    {
        case 0: // test
            #ifdef _USE_SOUND_
            engine.sounds.play("so001.wav");
            #endif
            return;
        default: return;
    }
}

#endif
