#define _USE_SOUND_
#ifdef _USE_SOUND_
#ifndef MANAGERSOUND_HPP
#define MANAGERSOUND_HPP

#include <SFML/Audio.hpp>
#include <map>
#include <vector>
#include <stdint.h>

class ManagerSound
{
    public:
        ManagerSound();
        ~ManagerSound();
        bool loadList(std::string name);
        bool unloadList(std::string name);
        sf::SoundBuffer* use(const std::string &name);
        void trash(const std::string &name);
        void trash(sf::SoundBuffer* ptr);
        void play(const std::string &name);
        void play(const std::string &name, float volume);
        void play(const std::string &name, const sf::Vector2i& pos);
        void play(const std::string &name, float volume, const sf::Vector2i& pos);
        void update(); // remove the stopped sounds from the memory
        void garbageCollection(); // remove the buffer not used (nUse <= 0)
        void pauseAll(); // pause all the sounds
        void playAll(); // play all the paused sounds

        void clear();
    protected:
        struct BufferContainer
        {
            sf::SoundBuffer* ptr;
            int32_t nUse;
        };
        std::map<std::string, BufferContainer> buffers;

        struct SoundContainer
        {
            sf::Sound* ptr;
            std::string buffer_name;
            bool hasPosition;
            sf::Vector2i pos;
        };
        std::vector<SoundContainer> sounds;
};
#endif

#endif // MANAGERSOUND_HPP
