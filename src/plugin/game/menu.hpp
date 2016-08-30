#ifndef MENU_HPP
#define MENU_HPP
#define _USE_MENU_
#ifdef _USE_MENU_

#include "../drawable/mdrawable.hpp"
#include "../../game/menu.hpp"

// possible states
#define MS_VISIBLE      0
#define MS_INVISIBLE    1
#define MS_TRANSPARENT  2
#define MS_ANIMATED     4
#define MS_CUSTOM       128

struct MenuVisual
{
    MDrawable* draw;
    MenuDataVisual* data;
};

struct MenuActor
{
    MenuVisual* v;
    MenuDataActor* data;
    bool selected;
};

class Menu
{
    public:
        Menu();
        ~Menu();
        void init(int32_t id);
        bool isInitialized() const; // if true, the menu loaded properly
        void action(); // check input
        void update(); // update animation etc...
        void draw(); // draw the menu items
        bool isPausingGame() const; // check if the game is paused when this menu is used
        void setCurrentChoice(int32_t n); // change the current selected choice

        static void initMenuScript();
    protected:
        void choiceChange(int8_t c);
        void processVisual(MenuVisual& ref, MenuDataVisual& visual);
        void processActor(MenuActor& ref, MenuDataActor& actor);
        void drawVisual(MenuVisual &ref, const sf::IntRect& view);
        void runScript(MenuActor& ref, const int32_t &id, const std::string& script);

        // defined in game_menu.cpp
        void initSpecial();
        void validChoice(const std::string& sound);
        void cancelChoice(const std::string& sound);
        void applyCustomRules(MenuActor& ref, const int32_t &id);

        bool pause;
        bool initialized;
        int32_t ID;
        int32_t cChoice;

        std::vector<MenuVisual> visuals;
        std::vector<MenuActor> choices;
        std::vector<MenuVisual> cursors;
        MenuData* data; // only for on-the-fly custom menu
};

#endif
#endif // MENU_HPP
