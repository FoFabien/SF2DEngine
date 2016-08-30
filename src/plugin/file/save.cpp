#include "save.hpp"
#ifdef _USE_SAVE_
#include "../../mlib/mlib.hpp"
#include "../../mlib/mcrypto.hpp"
#include "../../mlib/mrand.hpp"

#include "../../game/variable.hpp"

Save::Save()
{
    ready = false;
}

Save::~Save()
{
    //dtor
}

bool Save::init(const std::string& dir, const std::string& version, const int32_t maxSlot)
{
    if(maxSlot <= 0) return false;
    if(!mlib::dirExist(dir))
    {
        mlib::makeDir(dir);
        if(!mlib::dirExist(dir)) return false;
    }
    save_dir = dir;
    if(save_dir[save_dir.size()-1] == '\\') save_dir[save_dir.size()-1] = '/';
    else if(save_dir[save_dir.size()-1] != '/') save_dir += '/';

    version_string = version;
    max_slot = maxSlot;
    current_slot = -1;
    ready = true;

    refreshSlotState();
    return true;
}

bool Save::loadSystem()
{
    if(!ready) return false;
    std::ifstream f((save_dir + "system.sav").c_str(), std::ios::in | std::ios::binary);
    if(!f) return false;
    MCrypto fc(C_READ);
    fc.readSeed(f);

    std::string tmpStr;
    int32_t tmpInt, nInt, nStr;
    char c;

    do
    {
        if(!fc.good(f)) return false;
        fc.read(f, &c, 1);
        if(c != 0) tmpStr += c;
    }while(c != 0);

    if(tmpStr != version_string) return false;
    fc.read(f, (char*)&nInt, 4);
    fc.read(f, (char*)&nStr, 4);

    for(int32_t i = 0; i < nInt; ++i)
    {
        if(!fc.good(f)) return false;
        fc.read(f, (char*)&tmpInt, 4);
        if(i < SYSTEM_VARIABLE_COUNT) sysVar[i] = tmpInt;
    }

    for(int32_t i = 0; i < nStr; ++i)
    {
        tmpStr.clear();
        do
        {
            if(!fc.good(f)) return false;
            fc.read(f, &c, 1);
            if(c != 0) tmpStr += c;
        }while(c != 0);
        if(i < SYSTEM_STRING_COUNT) sysStr[i] = tmpStr;
    }

    f.close();
    return true;
}

bool Save::saveSystem()
{
    if(!ready) return false;
    std::ofstream f((save_dir + "system.sav").c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return false;
    MCrypto fc(C_WRITE);
    fc.setSeed(mrand::rand());

    std::string tmpStr;
    int32_t nInt, nStr;
    char c = 0;

    if(version_string.size() > 0) fc.write(f, version_string.c_str(), version_string.size());
    fc.write(f, &c, 1);
    nInt = SYSTEM_VARIABLE_COUNT;
    nStr = SYSTEM_STRING_COUNT;
    fc.write(f, (char*)&nInt, 4);
    fc.write(f, (char*)&nStr, 4);

    for(int32_t i = 0; i < SYSTEM_VARIABLE_COUNT; ++i)
        fc.write(f, (char*)&sysVar[i], 4);

    for(int32_t i = 0; i < SYSTEM_STRING_COUNT; ++i)
    {
        if(sysStr[i].size() > 0) fc.write(f, sysStr[i].c_str(), sysStr[i].size());
        fc.write(f, &c, 1);
    }

    fc.writeSeed(f);
    f.close();
    return true;
}

bool Save::loadSlot()
{
    return loadSlot(current_slot);
}

bool Save::loadSlot(int32_t s)
{
    if(!ready) return false;
    if(s < 0 || s >= max_slot) return false;

    std::ifstream f((save_dir + "save" + mlib::int2str(s) + ".sav").c_str(), std::ios::in | std::ios::binary);
    if(!f) return false;
    MCrypto fc(C_READ);
    fc.readSeed(f);

    std::string tmpStr;
    int32_t tmpInt, nInt, nStr;
    char c;

    do
    {
        if(!fc.good(f)) return false;
        fc.read(f, &c, 1);
        if(c != 0) tmpStr += c;
    }while(c != 0);

    if(tmpStr != version_string) return false;

    tmpStr.clear();
    do
    {
        if(!fc.good(f)) return false;
        fc.read(f, &c, 1);
        if(c != 0) tmpStr += c;
    }while(c != 0);
    states[s].header = tmpStr;

    fc.read(f, (char*)&nInt, 4);
    fc.read(f, (char*)&nStr, 4);

    for(int32_t i = 0; i < nInt; ++i)
    {
        if(!fc.good(f)) return false;
        fc.read(f, (char*)&tmpInt, 4);
        if(i < VARIABLE_COUNT) gVar[i] = tmpInt;
    }

    for(int32_t i = 0; i < nStr; ++i)
    {
        tmpStr.clear();
        do
        {
            if(!fc.good(f)) return false;
            fc.read(f, &c, 1);
            if(c != 0) tmpStr += c;
        }while(c != 0);
        if(i < STRING_COUNT) gStr[i] = tmpStr;
    }

    initDungeonData();
    for(int32_t i = 0; i < DUNGEON_MAX_COUNT; ++i)
    {
        if(!fc.good(f)) return false;
        fc.read(f, (char*)&tmpInt, 4);
        if(tmpInt < 0) return false;
        if(tmpInt > 0) fov[i].resize(tmpInt, 0);

        for(size_t j = 0; j < fov[i].size(); ++j)
        {
            if(!fc.good(f)) return false;
            fc.read(f, &c, 1);
            fov[i][j] = c;
        }
    }

    f.close();
    return true;
}

bool Save::saveSlot()
{
    return saveSlot(current_slot);
}

bool Save::saveSlot(int32_t s)
{
    if(!ready) return false;
    if(s < 0 || s >= max_slot) return false;

    std::ofstream f((save_dir + "save" + mlib::int2str(s) + ".sav").c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f) return false;
    MCrypto fc(C_WRITE);
    fc.setSeed(mrand::rand());

    std::string tmpStr;
    int32_t tmpInt, nInt, nStr;
    char c = 0;

    if(version_string.size() > 0) fc.write(f, version_string.c_str(), version_string.size());
    fc.write(f, &c, 1);

    if(states[s].header.size() > 0) fc.write(f, states[s].header.c_str(), states[s].header.size());
    fc.write(f, &c, 1);


    nInt = VARIABLE_COUNT;
    nStr = STRING_COUNT;
    fc.write(f, (char*)&nInt, 4);
    fc.write(f, (char*)&nStr, 4);

    for(int32_t i = 0; i < VARIABLE_COUNT; ++i)
        fc.write(f, (char*)&gVar[i], 4);

    for(int32_t i = 0; i < STRING_COUNT; ++i)
    {
        if(gStr[i].size() > 0) fc.write(f, gStr[i].c_str(), gStr[i].size());
        fc.write(f, &c, 1);
    }

    for(int32_t i = 0; i < DUNGEON_MAX_COUNT; ++i)
    {
        if(!fc.good(f)) return false;
        tmpInt = (int32_t)fov[i].size();
        fc.write(f, (char*)&tmpInt, 4);

        for(size_t j = 0; j < fov[i].size(); ++j)
        {
            if(!fc.good(f)) return false;
            c = fov[i][j];
            fc.write(f, &c, 1);
        }
    }

    fc.writeSeed(f);
    f.close();
    refreshSlotState();
    return true;
}

void Save::setSlot(int32_t s)
{
    if(s >= 0 && s < max_slot) current_slot = s;
}

void Save::refreshSlotState()
{
    if(!ready) return;
    std::string tmpStr;
    char c;
    bool error;
    states.clear();

    for(int32_t i = 0; i < max_slot; ++i)
    {
        states.push_back(SlotState());
        error = false;
        std::ifstream f((save_dir + "save" + mlib::int2str(i) + ".sav").c_str(), std::ios::in | std::ios::binary);
        if(f)
        {
            MCrypto fc(C_READ);
            fc.readSeed(f);
            tmpStr.clear();
            do
            {
                if(!fc.good(f)) error = true;
                fc.read(f, &c, 1);
                if(c != 0 && !error) tmpStr += c;
            }while(c != 0 && !error);

            if(tmpStr == version_string && !error)
            {
                tmpStr.clear();
                do
                {
                    if(!fc.good(f)) error = true;
                    fc.read(f, &c, 1);
                    if(c != 0 && !error) tmpStr += c;
                }while(c != 0 && !error);

                if(!error)
                {
                    states.back().header = tmpStr;
                    states.back().used = true;
                }
            }

            f.close();
        }
        else states.back().used = false;
    }
}

std::vector<SlotState> Save::getSlotState() const
{
    return states;
}

void Save::setCurrentSlotHeader(const std::string& header)
{
    if(current_slot >= 0 && current_slot < max_slot) states[current_slot].header = header;
}
#endif
