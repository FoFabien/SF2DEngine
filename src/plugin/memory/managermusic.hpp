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
        bool load(const std::string &name); // load the music
        void unload(const std::string &name); // mark the music for deletion
        void play(const std::string &name); // load the music (if it doesn't exist) and play it.
        void pause(const std::string &name); // pause the music (must exist)
        void stop(const std::string &name); // stop the music (must exist)

        std::vector<std::string> getMusicList() const; // return a list of the loaded files
        void setLoop(const std::string &name, bool b, int32_t start = -1, int32_t end = -1); // set the music's loop. start and end in milliseconds
        void garbageCollection();
        void update(); // apply loops
        void setVolume(const std::string &name, float v); // set the volume
        void updateVolume();
        void clear();
    protected:
        struct MusicContainer
        {
            Music *ptr;
            bool toDelete;
        };
        std::map<std::string, MusicContainer> data;
};
#endif

#endif // MANAGERMUSIC_H
