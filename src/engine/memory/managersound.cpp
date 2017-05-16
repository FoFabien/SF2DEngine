#include "managersound.hpp"

#ifdef _USE_SOUND_
#include "../engine.hpp"
#define MAX_SOUND_COUNT 150
#include "../mlib/mlib.hpp"

ManagerSound::ManagerSound()
{
    sounds.reserve(MAX_SOUND_COUNT);
}

ManagerSound::~ManagerSound()
{
    for(SoundContainer &i : sounds)
    {
        i.ptr->stop();
        delete i.ptr;
    }

    for(auto &i : buffers) delete i.second.ptr;
}

bool ManagerSound::loadList(std::string name)
{
    std::ifstream tmp1;
    std::stringstream tmp2;
    std::istream *f;

    #ifdef _USE_FILESYSTEM_
    if(engine.files.exist(name)) f = &(engine.files.getStringStream(name, tmp2));
    else
    #endif
    {
        tmp1.open(name.c_str(), std::ios::in);
        f = &tmp1;
    }

    if(!(*f)) return false;
    std::string str;
    while(f->good())
    {
        mlib::getline(*f, str);
        use(str);
    }
    return true;
}

bool ManagerSound::unloadList(std::string name)
{
    std::ifstream tmp1;
    std::stringstream tmp2;
    std::istream *f;

    #ifdef _USE_FILESYSTEM_
    if(engine.files.exist(name)) f = &(engine.files.getStringStream(name, tmp2));
    else
    #endif
    {
        tmp1.open(name.c_str(), std::ios::in);
        f = &tmp1;
    }

    if(!(*f)) return false;
    std::string str;
    while(f->good())
    {
        mlib::getline(*f, str);
        trash(str);
    }
    return true;
}

sf::SoundBuffer* ManagerSound::use(const std::string &name)
{
    std::map<std::string, BufferContainer>::iterator it = buffers.find(name);
    if(it == buffers.end())
    {
        BufferContainer tmp;
        tmp.nUse = 1;
        tmp.ptr = new sf::SoundBuffer();
        #ifdef _USE_FILESYSTEM_
        if(engine.files.loadFile(name)) // found in pack
        {
            sf::MemoryInputStream stream = engine.files.getStream(name);
            if(!tmp.ptr->loadFromStream(stream)) // try to load
            {
                if(!tmp.ptr->loadFromFile(name)) // failed, try to load from drive
                {
                    delete tmp.ptr;
                    return nullptr;
                }
            }
        }
        else if(!tmp.ptr->loadFromFile(name)) // or try to load from drive
        #else
        if(!tmp.ptr->loadFromFile(name)) // or try to load from drive
        #endif
        {
            delete tmp.ptr;
            return nullptr;
        }
        buffers[name] = tmp;
    }
    else ++buffers[name].nUse;
    return buffers[name].ptr;
}

void ManagerSound::trash(const std::string &name)
{
    std::map<std::string, BufferContainer>::iterator it = buffers.find(name);
    if(it != buffers.end()) --it->second.nUse;
}

void ManagerSound::trash(sf::SoundBuffer* ptr)
{
    for(auto &i : buffers)
    {
        if(i.second.ptr == ptr)
        {
            --i.second.nUse;
            return;
        }
    }
}

void ManagerSound::play(const std::string &name)
{
    if(name.empty()) return;
    if(sounds.size() >= MAX_SOUND_COUNT) return;
    sf::SoundBuffer *tmp = use(name);
    if(!tmp) return;
    SoundContainer s;
    s.hasPosition = false;
    s.buffer_name = name;
    s.ptr = new sf::Sound();
    s.ptr->setBuffer(*tmp);
    s.ptr->setVolume(engine.masterVolume);
    s.ptr->play();
    sounds.push_back(s);
}

void ManagerSound::play(const std::string &name, float volume)
{
    if(name.empty()) return;
    if(sounds.size() >= MAX_SOUND_COUNT) return;
    sf::SoundBuffer *tmp = use(name);
    if(!tmp) return;
    SoundContainer s;
    s.hasPosition = false;
    s.buffer_name = name;
    s.ptr = new sf::Sound();
    s.ptr->setBuffer(*tmp);
    s.ptr->setVolume(volume);
    s.ptr->play();
    sounds.push_back(s);
}

void ManagerSound::play(const std::string &name, const sf::Vector2i& pos)
{
    if(name.empty()) return;
    if(sounds.size() >= MAX_SOUND_COUNT) return;
    sf::SoundBuffer *tmp = use(name);
    if(!tmp) return;
    SoundContainer s;
    s.hasPosition = true;
    s.pos.x = pos.x;
    s.pos.y = pos.y;
    s.buffer_name = name;
    s.ptr = new sf::Sound();
    s.ptr->setBuffer(*tmp);
    s.ptr->setVolume(engine.masterVolume);
    s.ptr->play();
    sounds.push_back(s);
}

void ManagerSound::play(const std::string &name, float volume, const sf::Vector2i& pos)
{
    if(name.empty()) return;
    if(sounds.size() >= MAX_SOUND_COUNT) return;
    sf::SoundBuffer *tmp = use(name);
    if(!tmp) return;
    SoundContainer s;
    s.hasPosition = true;
    s.pos.x = pos.x;
    s.pos.y = pos.y;
    s.buffer_name = name;
    s.ptr = new sf::Sound();
    s.ptr->setBuffer(*tmp);
    s.ptr->setVolume(volume);
    s.ptr->play();
    sounds.push_back(s);
}

void ManagerSound::update()
{
    sf::Vector2f center = engine.getGameViewCenter();
    for(size_t i = 0; i < sounds.size(); ++i)
    {
        SoundContainer& ref = sounds[i];
        if(ref.ptr->getStatus() == sf::SoundSource::Stopped)
        {
            trash(ref.buffer_name);
            delete ref.ptr;
            sounds.erase(sounds.begin()+i);
            --i;
        }
        else if(ref.hasPosition)
        {
            ref.ptr->setPosition(ref.pos.x - center.x, 0.f, ref.pos.y - center.y);
        }
    }
}

void ManagerSound::garbageCollection()
{
    std::map<std::string, BufferContainer>::iterator it = buffers.begin();
    std::map<std::string, BufferContainer>::iterator tmp;
    while(it != buffers.end())
    {
        if(it->second.nUse <= 0)
        {
            delete it->second.ptr;
            tmp = it;
            ++it;
            buffers.erase(tmp);
        }
        else ++it;
    }
}

void ManagerSound::pauseAll()
{
    for(SoundContainer &i : sounds)
    {
        if(i.ptr->getStatus() == sf::SoundSource::Playing)
        {
            i.ptr->pause();
        }
    }
}

void ManagerSound::playAll()
{
    for(SoundContainer &i : sounds)
    {
        if(i.ptr->getStatus() == sf::SoundSource::Paused)
        {
            i.ptr->play();
        }
    }
}

void ManagerSound::clear()
{
    for(SoundContainer &i : sounds)
    {
        i.ptr->stop();
        delete i.ptr;
    }
    sounds.clear();

    for(auto &i : buffers) delete i.second.ptr;
    buffers.clear();
}
#endif
