#include "managermusic.hpp"

#ifdef _USE_MUSIC_
#include "../../engine.hpp"
#define MAX_MUSIC_COUNT 105

ManagerMusic::ManagerMusic()
{

}

ManagerMusic::~ManagerMusic()
{
    for(auto &i : data)
    {
        i.second.ptr->stop();
        delete (i.second.ptr);
    }
}

bool ManagerMusic::load(const std::string &name)
{
    std::map<std::string, MusicContainer>::iterator it = data.find(name);
    if(it == data.end())
    {
        MusicContainer tmp;
        tmp.ptr = new Music();
        if(!tmp.ptr->load(name))
        {
            delete tmp.ptr;
            return false;
        }
        data[name] = tmp;
    }
    data[name].toDelete = false;
    return true;
}

void ManagerMusic::unload(const std::string &name)
{
    std::map<std::string, MusicContainer>::iterator it = data.find(name);
    if(it != data.end())
        it->second.toDelete = true;
}

void ManagerMusic::play(const std::string &name)
{
    if(data.size() >= MAX_MUSIC_COUNT) return;

    if(load(name))
    {
        data[name].ptr->setVolume(engine.masterVolume);
        data[name].ptr->play();
    }
}

void ManagerMusic::pause(const std::string &name)
{
    if(data.find(name) != data.end())
    {
        data[name].ptr->pause();
    }
}

void ManagerMusic::stop(const std::string &name)
{
    if(data.find(name) != data.end())
    {
        data[name].ptr->stop();
    }
}

std::vector<std::string> ManagerMusic::getMusicList() const
{
    std::vector<std::string> l;
    l.reserve(data.size());
    for(auto &i : data)
        l.push_back(i.first);
    return l;
}

void ManagerMusic::setLoop(const std::string &name, bool b, int32_t start, int32_t end)
{
    if(data.find(name) != data.end())
    {
        data[name].ptr->loop(b, start, end);
    }
}

void ManagerMusic::garbageCollection()
{
    std::map<std::string, MusicContainer>::iterator it = data.begin();
    std::map<std::string, MusicContainer>::iterator tmp;
    while(it != data.end())
    {
        if(it->second.toDelete)
        {
            delete it->second.ptr;
            tmp = it;
            ++it;
            data.erase(tmp);
        }
        else ++it;
    }
}

void ManagerMusic::update()
{
    for(auto &i : data) i.second.ptr->update();
}

void ManagerMusic::setVolume(const std::string &name, float v)
{
    if(data.find(name) != data.end())
    {
        data[name].ptr->setVolume(v);
    }
}

void ManagerMusic::updateVolume()
{
    for(auto &i : data)
    {
        i.second.ptr->setVolume(engine.masterVolume);
    }
}

void ManagerMusic::clear()
{
    for(auto &i : data)
    {
        i.second.ptr->stop();
        delete (i.second.ptr);
    }
    data.clear();
}
#endif
