#include "music.hpp"
#ifdef _USE_MUSIC_
#include "../../engine.hpp"

Music::Music()
{
    music = nullptr;
    loaded = false;
    loop_start_offset = 0;
    loop_end_offset = 0;
    toLoop = false;
}

Music::~Music()
{
    if(music)
    {
        music->stop();
        delete music;
        #ifdef _USE_FILESYSTEM_
        engine.files.unlockFile(music_name);
        engine.files.unloadFile(music_name);
        #endif
    }
}

bool Music::load(std::string name)
{
    if(loaded) return false;

    music = new sf::Music();
    music_name = name;
    #ifdef _USE_FILESYSTEM_
    if(engine.files.loadFile(name)) // found in pack
    {
        stream_data = engine.files.getStream(name);
        if(!music->openFromStream(stream_data)) // try to load
        {
            if(!music->openFromFile(name)) // failed, try to load from drive
            {
                delete music;
                music = nullptr;
                return false;
            }
        }
        engine.files.lockFile(name);
    }
    else if(!music->openFromFile(name)) // or try to load from drive
    #else
    if(!music->openFromFile(name)) // or try to load from drive
    #endif
    {
        delete music;
        music = nullptr;
        return false;
    }
    loaded = true;
    return true;
}

void Music::play()
{
    if(music) music->play();
}

void Music::pause()
{
    if(music) music->pause();
}

void Music::stop()
{
    if(music) music->stop();
}

void Music::loop(bool b, int32_t start, int32_t end)
{
    if(!music) return;
    toLoop = b;
    if(start <= -1) start = 0;
    if(end <= -1) end = music->getDuration().asMilliseconds() - 200;
    loop_start_offset = start;
    loop_end_offset = end;
}

void Music::update()
{
    if(music && toLoop && music->getStatus() == sf::SoundSource::Playing)
    {
        if(music->getPlayingOffset().asMilliseconds() >= (int32_t)loop_end_offset)
        {
            //Out = "Music loop: " + mlib::dec2str(music->getPlayingOffset().asMilliseconds()) + "->" + mlib::dec2str(loop_start_offset) + "\n";
            music->setPlayingOffset(sf::milliseconds(loop_start_offset));
        }
    }
}

void Music::setVolume(float v)
{
    if(music) music->setVolume(v);
}

sf::SoundSource::Status Music::getStatus()
{
    if(!music) return sf::SoundSource::Stopped;
    return music->getStatus();
}
#endif
