#include "managertexture.hpp"
#ifdef _USE_TEXTURE_
#include "../../engine.hpp"

ManagerTexture::ManagerTexture()
{
    //ctor
}

ManagerTexture::~ManagerTexture()
{
    for(auto &i : data) delete i.second.ptr;
}

bool ManagerTexture::exist(const std::string &name)
{
    return (data.find(name) != data.end());
}

sf::Texture* ManagerTexture::use(const std::string &name)
{
    std::map<std::string, TextureContainer>::iterator it = data.find(name);
    if(it == data.end())
    {
        TextureContainer tmp;
        tmp.ptr = new sf::Texture();
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
        tmp.nUse = 1;
        data[name] = tmp;
    }
    else ++data[name].nUse;
    return data[name].ptr;
}

void ManagerTexture::trash(const std::string &name)
{
    std::map<std::string, TextureContainer>::iterator it = data.find(name);
    if(it != data.end()) --(it->second.nUse);
}

void ManagerTexture::trash(const sf::Texture* ptr)
{
    for(auto &i : data)
    {
        if(i.second.ptr == ptr)
        {
            --(i.second.nUse);
            return;
        }
    }
}

void ManagerTexture::garbageCollection()
{
    std::map<std::string, TextureContainer>::iterator it = data.begin();
    std::map<std::string, TextureContainer>::iterator tmp;
    while(it != data.end())
    {
        if(it->second.nUse <= 0)
        {
            delete it->second.ptr;
            tmp = it;
            ++it;
            data.erase(tmp);
        }
        else ++it;
    }
}

void ManagerTexture::clear()
{
    for(auto &i : data) delete i.second.ptr;
    data.clear();
}
#endif
