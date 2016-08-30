#ifndef GAME_VARIABLE_H
#define GAME_VARIABLE_H

#include <string>
#include <vector>
#include <stdint.h>

// globale variable
#define VARIABLE_COUNT 100
extern int32_t gVar[VARIABLE_COUNT];
#define SYSTEM_VARIABLE_COUNT 50
extern int32_t sysVar[SYSTEM_VARIABLE_COUNT];
void initGlobaleVariable();
void resetGlobaleVariableAndString(); /// reset gVar and gStr
void storeFloat(const float &f, int32_t &v);
float loadFloat(const int32_t &v);

// globale string
#define STRING_COUNT 10
extern std::string  gStr[STRING_COUNT];
void updateGlobaleString(int32_t id = -1);
#define SYSTEM_STRING_COUNT 10
extern std::string  sysStr[SYSTEM_STRING_COUNT];
void updateSystemString(int32_t id = -1);

// debug
#ifdef DEBUG_BUILD
extern int32_t dVar[10];
extern std::string dStr[10];
#endif

// game define
#define GV_GAMESTATE    0
#define GV_MAPID        1
#define GV_PLAYERX      2
#define GV_PLAYERY      3

// ---------------------------------
// add specific global variable here
// ---------------------------------
#define DUNGEON_MAX_COUNT 100
extern std::vector<std::vector<int8_t> > fov;
extern std::vector<int8_t> tile_grid;
void initDungeonData();

#endif // GAME_VARIABLE_H
