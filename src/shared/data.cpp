#include <string>
#include <stdint.h>

#define USE_SETTING_DATA
#include "data.hpp"

// entity
std::string entity_test_database[ENTITY_COUNT]=
{
    "test1",
    "test2",
    "dungeon_player",
    "dungeon_monster"
};

int8_t settWinS;
int32_t settVMode;
int32_t settWinX;
int32_t settWinY;
int32_t settFPS;
int32_t settVol;
int32_t settLang;

bool settChangingKey;
int32_t settKeyCode;
