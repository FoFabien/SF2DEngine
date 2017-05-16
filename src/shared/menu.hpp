#ifndef GAME_MENU_H
#define GAME_MENU_H

#include <string>
#include <vector>
#include <stdint.h>
#include <SFML/Graphics/Color.hpp>

// menu data
#define SPECIAL_TYPE_OFFSET 10
struct MenuDataVisual
{
    MenuDataVisual()
    : type(0), s(""), fixed(0), offsetX(0), offsetY(0), size1(0), size2(0), color(sf::Color::White), s1(""), s2("") {};

    MenuDataVisual(int8_t type, const std::string &s, int8_t fixed, int32_t offsetX, int32_t offsetY, int32_t size1, int32_t size2, sf::Color color, const std::string &s1, const std::string &s2)
    : type(type), s(s), fixed(fixed), offsetX(offsetX), offsetY(offsetY), size1(size1), size2(size2), color(color), s1(s1), s2(s2) {};

	int8_t type; // type of graphic elements (use MDrawable type + special types starting from value SPECIAL_TYPE_OFFSET)
	std::string s; // string who will be used by the MDrawable
	int8_t fixed; // relative position
	int32_t offsetX; // position
	int32_t offsetY;
	int32_t size1; // size (for some MDrawable type)
	int32_t size2;
	sf::Color color; // color (for some MDrawable type)
	std::string s1; // additional string 1
	std::string s2; // additional string 2
};

struct MenuDataActor
{
    MenuDataActor() {};
    MenuDataActor(MenuDataVisual v, const std::string &default_state, const std::string &unselec_state, const std::string &selec_state, int8_t unselec_key, int8_t selec_key, int8_t up, int8_t right, int8_t down, int8_t left,
                  const std::string &choice_sound, const std::string &unselec_sound, const std::string &selec_sound)
    : v(v), default_state(default_state), unselec_state(unselec_state), selec_state(selec_state), unselec_key(unselec_key), selec_key(selec_key), up(up), right(right), down(down), left(left), choice_sound(choice_sound), unselec_sound(unselec_sound), selec_sound(selec_sound) {};

	MenuDataVisual v; // graphical part
	std::string default_state; // what to do at the creation
	std::string unselec_state; // what to do when it's unselected
	std::string selec_state; // what to do when it's selected
	int8_t unselec_key; // the key used to cancel
	int8_t selec_key; // the key used to confirm
	int8_t up; // choice to go when the up key is pressed
	int8_t right; // etc...
	int8_t down;
	int8_t left;
	std::string choice_sound; // sound when changing choice
	std::string unselec_sound; // sound when pressing cancel
	std::string selec_sound; // sound when pressing confirm
};

#define DEFT_MENU_VSIZE 5
struct MenuData // raw data used to create a menu
{
    MenuData() {visuals.reserve(DEFT_MENU_VSIZE); choices.reserve(DEFT_MENU_VSIZE);};
    MenuData(const bool &pause, const MenuDataVisual &cursor): pause(pause) { cursors.push_back(cursor); visuals.reserve(DEFT_MENU_VSIZE); choices.reserve(DEFT_MENU_VSIZE);};
    MenuData(const bool &pause, const std::vector<MenuDataVisual> &cursors): pause(pause), cursors(cursors) {visuals.reserve(DEFT_MENU_VSIZE); choices.reserve(DEFT_MENU_VSIZE);};
    bool pause;
	std::vector<MenuDataVisual> cursors; // cursors (can have multiple ones)
	std::vector<MenuDataVisual> visuals; // various graphical elements
	std::vector<MenuDataActor> choices; // default choices available
};

extern std::vector<MenuData> menu_database; // list of all menus
void initMenuData();

#endif // GAME_MENU_H
