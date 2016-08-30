#ifndef MANAGERMUSIC_HPP
#define MANAGERMUSIC_HPP

#include "../system/music.hpp"
#ifdef _USE_MUSIC_
#include <map>

class ManagerMusic
{
    public:
        ManagerMusic();
        ~ManagerMusic();
        void play(const std::string &name); // load the music and play it. The last music will be paused
        void pause(const std::string &name); // pause the music
        void stop(const std::string &name); // stop the music
        void stopAllNotCurrent(); // stop all the music except the current one
        void setLoop(const std::string &name, bool b, int32_t start = -1, int32_t end = -1); // set the music's loop. start and end in milliseconds
        void garbageCollection(); // remove all the stopped music from the memory
        void update(); // apply the loop to the current music
        void setVolume(const std::string &name, float v); // set the volume

        // same as above but apply to the current music if there is one
        void play();
        void pause();
        void stop();
        void setLoop(bool b, int32_t start = -1, int32_t end = -1);
        void setVolume(float v);

        void updateVolume();
        void clear();
    protected:
        std::map<std::string, Music*> data;
        Music *current;
};
#endif

#endif // MANAGERMUSIC_H
