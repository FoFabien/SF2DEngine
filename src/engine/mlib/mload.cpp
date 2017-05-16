#include "mload.hpp"
#include "mlib.hpp"
#ifndef MAP_EDITOR
#include "../engine.hpp"
#endif

MLoad::MLoad()
{
    filename = "";
}

MLoad::~MLoad()
{
    //dtor
}

bool MLoad::loadFromMemory(std::string file, bool tryFromFile)
{
    #ifndef MAP_EDITOR
    #ifdef _USE_FILESYSTEM_
    if(!engine.files.loadFile(file))
    #endif
    {
        if(tryFromFile)
        {
            #ifdef DEBUG_BUILD
            Out = "Loading \"" + file + "\" from the hard drive...\n";
            #endif
            return loadFromFile(file);
        }
        return false;
    }

    #ifdef _USE_FILESYSTEM_
    std::stringstream ss;
    if(!load(engine.files.getStringStream(file,ss)))
    {
        #ifdef DEBUG_BUILD
        Out = "Failed to load the file \"" + file + "\"\n";
        #endif
        return false;
    }
    filename = file;
    return true;
    #endif
    #else
    return loadFromFile(file);
    #endif
}

bool MLoad::loadFromFile(std::string file)
{
    std::ifstream f(file.c_str(), std::ios::in | std::ios::binary);
    if(!f)
    {
        #ifdef DEBUG_BUILD
        Out = "Failed to open \"" + file + "\"\n";
        #endif
        return false;
    }
    if(!load(f))
    {
        #ifdef DEBUG_BUILD
        Out = "Failed to load the file \"" + file + "\"\n";
        #endif
        return false;
    }
    filename = file;
    return true;
}
