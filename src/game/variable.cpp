#include "variable.hpp"

// globale variable
int32_t gVar[VARIABLE_COUNT];
int32_t sysVar[SYSTEM_VARIABLE_COUNT];
void initGlobaleVariable()
{
    for(int32_t i = 0; i < VARIABLE_COUNT; ++i) gVar[i] = 0;
    for(int32_t i = 0; i < SYSTEM_VARIABLE_COUNT; ++i) sysVar[i] = 0;
}

void resetGlobaleVariableAndString()
{
    for(int32_t i = 0; i < VARIABLE_COUNT; ++i) gVar[i] = 0;
    for(int32_t i = 0; i < STRING_COUNT; ++i) gStr[i].clear();
}

void storeFloat(const float &f, int32_t &v)
{
    v = *((int32_t*)(&f));
}

float loadFloat(const int32_t &v)
{
    return *((float*)(&v));
}

// globale string
std::string gStr[STRING_COUNT];
void updateGlobaleString(int32_t id)
{
    if(id < 0)
    {
        for(int32_t i = 0; i < STRING_COUNT; ++i) updateGlobaleString(i);
        return;
    }
    if(id > STRING_COUNT) return;
    switch(id)
    {
        default: return;
    }
}

std::string sysStr[SYSTEM_STRING_COUNT];
void updateSystemString(int32_t id)
{
    if(id < 0)
    {
        for(int32_t i = 0; i < SYSTEM_STRING_COUNT; ++i) updateSystemString(i);
        return;
    }
    if(id > SYSTEM_STRING_COUNT) return;
    switch(id)
    {
        default: return;
    }
}

// debug
#ifdef DEBUG_BUILD
    int32_t dVar[10];
    std::string dStr[10];
#endif

// ---------------------------------
// add specific global variable here
// ---------------------------------
// dungeon data
std::vector<std::vector<int8_t> > fov;
std::vector<int8_t> tile_grid;
void initDungeonData()
{
    fov.clear();
    while(fov.size() < DUNGEON_MAX_COUNT) fov.push_back(std::vector<int8_t>());
    tile_grid.clear();
}

