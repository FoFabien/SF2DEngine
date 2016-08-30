#ifndef MUSIC_HPP
#define MUSIC_HPP
#define _USE_MUSIC_
#ifdef _USE_MUSIC_

#include <stdint.h>
#include <SFML/Audio/Music.hpp>
#include <SFML/System/MemoryInputStream.hpp>
#include "../file/filesystem.hpp"

class Music
{
    public:
        Music();
        ~Music();
        bool load(std::string name); // load the music file
        void play();
        void pause();
        void stop();
        void loop(bool b, int32_t start = -1, int32_t end = -1); // set the loop. start and end are in milliseconds
        void update(); // apply the loop if enabled
        void setVolume(float v);
        sf::SoundSource::Status getStatus(); // return the music state
    protected:
        sf::MemoryInputStream stream_data;
        sf::Music *music;
        bool loaded;
        uint32_t loop_start_offset;
        uint32_t loop_end_offset;
        bool toLoop;
        std::string music_name;
};

#endif
#endif // MUSIC_HPP
