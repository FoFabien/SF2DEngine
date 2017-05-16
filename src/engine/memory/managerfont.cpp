#include "managerfont.hpp"
#ifdef _USE_FONT_
#include "../engine.hpp"

ManagerFont::ManagerFont()
{

}

ManagerFont::~ManagerFont()
{
    for(auto &i : data)
    {
        delete (i.second->ptr);
        #ifdef _USE_FILESYSTEM_
        engine.files.unlockFile(i.first);
        engine.files.unloadFile(i.first);
        #endif
        delete (i.second);
    }
}

bool ManagerFont::exist(const std::string &name)
{
    return (data.find(name) != data.end());
}

const sf::Font* ManagerFont::use(const std::string &name)
{
    std::map<std::string, FontContainer*>::iterator it = data.find(name);
    if(it == data.end())
    {
        FontContainer* tmp = new FontContainer();
        tmp->ptr = new sf::Font();

        #ifdef _USE_FILESYSTEM_
        if(engine.files.loadFile(name))
        {
            tmp->stream_data = engine.files.getStream(name);
            if(!tmp->ptr->loadFromStream(tmp->stream_data))
            {
                delete tmp->ptr;
                delete tmp;
                return nullptr;
            }
            engine.files.lockFile(name);
        }
        else if(!tmp->ptr->loadFromFile(name))
        #else
        if(!tmp->ptr->loadFromFile(name))
        #endif
        {
            delete tmp->ptr;
            delete tmp;
            return nullptr;
        }
        tmp->nUse = 1;

        data[name] = tmp;
    }
    else ++data[name]->nUse;
    return data[name]->ptr;
}

void ManagerFont::trash(const std::string &name)
{
    std::map<std::string, FontContainer*>::iterator it = data.find(name);
    if(it != data.end()) --(it->second->nUse);
}

void ManagerFont::trash(const sf::Font* ptr)
{
    for(auto &i : data)
    {
        if(ptr == i.second->ptr)
        {
            --(i.second->nUse);
            return;
        }
    }
}

void ManagerFont::garbageCollection()
{
    std::map<std::string, FontContainer*>::iterator it = data.begin();
    std::map<std::string, FontContainer*>::iterator tmp;
    while(it != data.end())
    {
        if(it->second->nUse <= 0)
        {
            delete (it->second->ptr);
            #ifdef _USE_FILESYSTEM_
            engine.files.unlockFile(it->first);
            engine.files.unloadFile(it->first);
            #endif
            delete (it->second);
            tmp = it;
            ++it;
            data.erase(tmp);
        }
        else ++it;
    }
}

void ManagerFont::setCurrentFont(const std::string &name)
{
    currentFont = name;
}

const sf::Font* ManagerFont::useCurrentFont()
{
    return use(currentFont);
}

void ManagerFont::trashCurrentFont()
{
    trash(currentFont);
}

void ManagerFont::clear()
{
    for(auto &i : data)
    {
        delete (i.second->ptr);
        #ifdef _USE_FILESYSTEM_
        engine.files.unlockFile(i.first);
        engine.files.unloadFile(i.first);
        #endif
    }
    data.clear();
}
#endif
