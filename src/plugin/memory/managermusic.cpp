#include "managermusic.hpp"

#ifdef _USE_MUSIC_
#include "../../engine.hpp"
#define MAX_MUSIC_COUNT 105

ManagerMusic::ManagerMusic()
{
    current = nullptr;
}

ManagerMusic::~ManagerMusic()
{
    for(auto &i : data)
    {
        i.second->stop();
        delete (i.second);
    }
}

void ManagerMusic::play(const std::string &name)
{
    std::map<std::string, Music*>::iterator it = data.find(name);
    if(it != data.end())
    {
        if(it->second != current)
        {
            if(current) current->pause();
            current = it->second;
            current->setVolume(engine.masterVolume);
            current->play();
        }
        return;
    }
    if(data.size() >= MAX_MUSIC_COUNT) return;
    Music* ptr = new Music();
    if(!ptr->load(name))
    {
        delete ptr;
        return;
    }
    data[name] = ptr;
    ptr->setVolume(engine.masterVolume);
    ptr->play();
    if(current) current->pause();
    current = ptr;
}

void ManagerMusic::pause(const std::string &name)
{
    if(data.find(name) != data.end())
    {
        data[name]->pause();
    }
}

void ManagerMusic::stop(const std::string &name)
{
    if(data.find(name) != data.end())
    {
        data[name]->stop();
    }
}

void ManagerMusic::stopAllNotCurrent()
{
    for(auto &i : data)
    {
        if(i.second != current)
        {
            i.second->stop();
        }
    }
}

void ManagerMusic::setLoop(const std::string &name, bool b, int32_t start, int32_t end)
{
    if(data.find(name) != data.end())
    {
        data[name]->loop(b, start, end);
    }
}

void ManagerMusic::garbageCollection()
{
    std::map<std::string, Music*>::iterator it = data.begin();
    std::map<std::string, Music*>::iterator tmp;
    while(it != data.end())
    {
        if(it->second->getStatus() == sf::SoundSource::Stopped)
        {
            if(current == it->second) current = nullptr;
            delete it->second;
            tmp = it;
            ++it;
            data.erase(tmp);
        }
        else ++it;
    }
}

void ManagerMusic::update()
{
    if(current) current->update();
}

void ManagerMusic::setVolume(const std::string &name, float v)
{
    if(data.find(name) != data.end())
    {
        data[name]->setVolume(v);
    }
}

void ManagerMusic::play()
{
    if(current) current->play();
}

void ManagerMusic::pause()
{
    if(current) current->pause();
}

void ManagerMusic::stop()
{
    if(current) current->stop();
}

void ManagerMusic::setLoop(bool b, int32_t start, int32_t end)
{
    if(current) current->loop(b, start, end);
}

void ManagerMusic::setVolume(float v)
{
    if(current) current->setVolume(v);
}

void ManagerMusic::updateVolume()
{
    for(auto &i : data)
    {
        i.second->setVolume(engine.masterVolume);
    }
}

void ManagerMusic::clear()
{
    for(auto &i : data)
    {
        i.second->stop();
        delete (i.second);
    }
    data.clear();
}
#endif
