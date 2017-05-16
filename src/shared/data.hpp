#ifndef GAME_DATA_HPP
#define GAME_DATA_HPP

#include <string>
#include <stdint.h>


// ---------------------------------
// general data
// ---------------------------------

// entity
#define ENTITY_COUNT 4
extern std::string entity_test_database[ENTITY_COUNT];


#ifdef USE_SETTING_DATA
extern int8_t settWinS;
extern int32_t settVMode;
extern int32_t settFPS;
extern int32_t settVol;
extern int32_t settLang;

extern bool settChangingKey;
extern int32_t settKeyCode;
#endif

#endif // GAME_DATA_HPP
