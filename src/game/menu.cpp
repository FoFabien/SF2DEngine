#include "menu.hpp"

#include "../plugin/game/menu.hpp"

#define SP_UNDEF_DRAW   0+SPECIAL_TYPE_OFFSET
#define SP_TEXT         1+SPECIAL_TYPE_OFFSET
#define SP_SPRITE       2+SPECIAL_TYPE_OFFSET
#define SP_ANIMATION    3+SPECIAL_TYPE_OFFSET
#define SP_RECTANGLE    4+SPECIAL_TYPE_OFFSET
#define SP_CIRCLE       5+SPECIAL_TYPE_OFFSET
#define SP_STDTEXT      6+SPECIAL_TYPE_OFFSET
#define SP_GAMETEXT     7+SPECIAL_TYPE_OFFSET


// menu data
#include "../plugin/system/input.hpp"
std::vector<MenuData> menu_database;

void initMenuData()
{
    #ifdef _USE_MENU_
    #ifdef _USE_INPUT_
    // menu 0
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_CIRCLE, "", 0, -20, 2, 7, 10, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -51, -51, 122, 102, sf::Color(0, 0, 0, 150), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -50, -50, 120, 100, sf::Color(150, 100, 100, 150), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -20, -40, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "custom", "custom",
                                                         KCOUNT, KSTART, 3, -1, 1, -1, "so001.wav", "", "")); // TEST_MENU0
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -20, -20, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "custom", "custom",
                                                         KCOUNT, KSTART, 0, -1, 2, -1, "so001.wav", "", "")); // TEST_MENU1
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -20, 0, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "custom", "custom",
                                                         KCOUNT, KSTART, 1, -1, 3, -1, "so001.wav", "", "")); // TEST_MENU2
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "TEST_MENU5", 5, -20, 20, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 2, -1, 0, -1, "so001.wav", "", "so001.wav")); // TEST_MENU5
    // menu 1
    menu_database.push_back(MenuData(false, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, 19, -51, 152, 67, sf::Color(0, 0, 0, 150), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, 20, -50, 150, 65, sf::Color(100, 100, 150, 150), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_GAMETEXT, "", 5, 50, -40, 15, 50, sf::Color::White, "", ""),
                                                         "animation=1;custom", "animation=1;custom", "animation=1;custom",
                                                         KCOUNT, KSTART, 1, -1, 1, -1, "so001.wav", "", "")); // TEST_MENU3
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_GAMETEXT, "", 5, 50, -20, 15, 100, sf::Color::White, "", ""),
                                                         "animation=1;custom", "animation=1;custom", "animation=1;custom",
                                                         KCOUNT, KSTART, 0, -1, 0, -1, "so001.wav", "", "")); // TEST_MENU4

    // menu 2 - TITLE
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_CIRCLE, "", 0, -20, 2, 7, 10, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -51, -51, 122, 102, sf::Color(0, 0, 0, 150), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -50, -50, 120, 100, sf::Color(200, 100, 100, 200), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -20, -40, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "custom", "custom",
                                                         KCOUNT, KSTART, 3, -1, 1, -1, "so001.wav", "", "")); // TITLE 0
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -20, -20, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "custom", "custom",
                                                         KCOUNT, KSTART, 0, -1, 2, -1, "so001.wav", "", "")); // TITLE 1
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -20, 0, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "custom", "custom",
                                                         KCOUNT, KSTART, 1, -1, 3, -1, "so001.wav", "", "")); // TITLE 2
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -20, 20, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "custom", "custom",
                                                         KCOUNT, KSTART, 2, -1, 0, -1, "so001.wav", "", "")); // TITLE 3
    // menu 3 - options
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, 24, -51, 122, 102, sf::Color(0, 0, 0, 150), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, 25, -50, 120, 100, sf::Color(100, 100, 200, 200), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "general", 5, 55, -40, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 2, -1, 1, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "key binding", 5, 55, -20, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 0, -1, 2, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "back", 5, 55, 0, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 1, -1, 0, -1, "so001.wav", "", ""));
    // menu 4 - settings
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -200, -200, 400, 400, sf::Color(50, 50, 100, 255), "", ""));

    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "window mode", 5, -170, -190, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 6, -1, 1, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "fullscreen resolution", 5, -170, -170, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 0, -1, 2, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "fps limit", 5, -170, -150, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 1, -1, 3, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "master volume", 5, -170, -130, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 2, -1, 4, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "language", 5, -170, -110, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 3, -1, 5, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "apply", 5, -170, -70, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 4, -1, 6, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "cancel", 5, -170, -50, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 5, -1, 0, -1, "so001.wav", "", ""));
    // menu 5 - window setting
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -151, -196, 242, 97, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -150, -195, 240, 95, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "window", 5, -110, -190, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 3, -1, 1, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "fullscreen", 5, -110, -170, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 0, -1, 2, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "borderless", 5, -110, -150, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 1, -1, 3, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "back", 5, -110, -130, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 2, -1, 0, -1, "so001.wav", "", ""));
    // menu 6 - resolution
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -151, -176, 242, 37, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -150, -175, 240, 35, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -110, -170, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, -1, -1, -1, -1, "so001.wav", "", ""));
    // menu 7 - fps limit
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -151, -156, 242, 197, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -150, -155, 240, 195, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "30", 5, -110, -150, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 8, -1, 1, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "60", 5, -110, -130, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 0, -1, 2, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "90", 5, -110, -110, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 1, -1, 3, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "120", 5, -110, -90, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 2, -1, 4, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "144", 5, -110, -70, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 3, -1, 5, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "160", 5, -110, -50, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 4, -1, 6, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "240", 5, -110, -30, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 5, -1, 7, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "300", 5, -110, -10, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 6, -1, 8, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "back", 5, -110, 10, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 7, -1, 0, -1, "so001.wav", "", ""));
    // menu 8 - master sound volume
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -151, -156, 242, 57, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -150, -155, 240, 55, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "< ? >", 5, -110, -150, 15, 0, sf::Color::White, "", ""),
                                                         "custom", "", "custom",
                                                         KLEFT, KRIGHT, 1, -1, 1, -1, "so001.wav", "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "back", 5, -110, -130, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, 0, -1, 0, -1, "so001.wav", "", ""));
    // menu 9 - key binding
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -200, -200, 400, 35, sf::Color(50, 50, 100, 255), "", ""));

    menu_database.back().visuals.push_back(MenuDataVisual(SP_TEXT, "", 5, -80, -190, 15, 0, sf::Color::White, "", "")); // keys
    menu_database.back().visuals.push_back(MenuDataVisual(SP_TEXT, "", 5, 0, -190, 15, 0, sf::Color::White, "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_TEXT, "", 5, 80, -190, 15, 0, sf::Color::White, "", ""));

    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -170, -190, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, -1, -1, -1, -1, "so001.wav", "", ""));
    // menu 10 - waiting for key
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_UNDEF_DRAW, "", 0, 0, 0, 0, 0, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -76, -18, 202, 37, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -75, -17, 200, 35, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "waiting input...", 5, -60, -10, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KCOUNT, -1, -1, -1, -1, "so001.wav", "", ""));
    // menu 11 - new game
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -121, -46, 242, 37, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -120, -45, 240, 35, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -80, -40, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, -1, -1, -1, -1, "so001.wav", "", ""));
    // menu 12 - new game
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -121, -46, 242, 37, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -120, -45, 240, 35, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -80, -40, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, -1, -1, -1, -1, "so001.wav", "", ""));
    // menu 13 - language
    menu_database.push_back(MenuData(true, MenuDataVisual(SP_RECTANGLE, "", 0, -20, 2, 14, 14, sf::Color::White, "", "")));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -151, -176, 242, 37, sf::Color(0, 0, 0, 255), "", ""));
    menu_database.back().visuals.push_back(MenuDataVisual(SP_RECTANGLE, "", 5, -150, -175, 240, 35, sf::Color(50, 100, 50, 255), "", ""));
    menu_database.back().choices.push_back(MenuDataActor(MenuDataVisual(SP_TEXT, "", 5, -110, -170, 15, 0, sf::Color::White, "", ""),
                                                         "", "", "",
                                                         KCOUNT, KSTART, -1, -1, -1, -1, "so001.wav", "", ""));
    #endif
    #endif
}

#ifdef _USE_MENU_
#include "../engine.hpp"
#include "../mlib/mlib.hpp"
#define USE_SETTING_DATA
#include "data.hpp"
#include "variable.hpp"

void Menu::initSpecial()
{
    switch(ID)
    {
        case 6:
        {
            std::vector<std::string> strings;
            data = new MenuData();
            *data = menu_database[ID];

            engine.refreshVideoMode();
            const std::vector<sf::VideoMode> modes = engine.getVideoModeList();
            for(const sf::VideoMode &i : modes)
                strings.push_back(mlib::int2str(i.width) + "x" + mlib::int2str(i.height));
            strings.push_back("back");

            data->visuals[0].size2 += (strings.size() - 1) * 20;
            data->visuals[1].size2 += (strings.size() - 1) * 20;

            data->choices[0].v.s = strings[0];
            data->choices[0].up = strings.size()-1;
            data->choices[0].down = 1;
            for(size_t i = 1; i < strings.size(); ++i)
            {
                data->choices.push_back(data->choices[0]);
                data->choices.back().v.s = strings[i];
                data->choices.back().v.offsetY += 20 * i;
                data->choices.back().up = i-1;
                data->choices.back().down = (i+1)%strings.size();
            }
            break;
        }
        case 9:
        {
            #ifdef _USE_INPUT_
            std::vector<std::string> strings;
            strings.push_back("Up"); // test
            strings.push_back("Right");
            strings.push_back("Down");
            strings.push_back("Left");
            strings.push_back("Shoot");
            strings.push_back("Jump");
            strings.push_back("Run");
            strings.push_back("Start");
            strings.push_back("Select");
            strings.push_back("LeftClick");

            while(strings.size() < KCOUNT) strings.push_back("???");

            data = new MenuData();
            *data = menu_database[ID];

            strings.push_back("back");

            data->visuals[0].size2 += (strings.size() - 1) * 20;

            // key strings
            data->visuals[1].s = engine.input.getKeyString(0);
            data->visuals[2].s = engine.input.getMouseString(0);
            data->visuals[3].s = engine.input.getPadButtonString(0);
            for(uint32_t i = 1; i < strings.size()-1; ++i)
            {
                data->visuals.push_back(data->visuals[1]);
                data->visuals.back().s = engine.input.getKeyString(i);
                data->visuals.back().offsetY += 20 * i;

                data->visuals.push_back(data->visuals[2]);
                data->visuals.back().s = engine.input.getMouseString(i);
                data->visuals.back().offsetY += 20 * i;

                data->visuals.push_back(data->visuals[3]);
                data->visuals.back().s = engine.input.getPadButtonString(i);
                data->visuals.back().offsetY += 20 * i;
            }

            data->choices[0].v.s = strings[0];
            data->choices[0].up = strings.size()-1;
            data->choices[0].down = 1;
            for(size_t i = 1; i < strings.size(); ++i)
            {
                data->choices.push_back(data->choices[0]);
                data->choices.back().v.s = strings[i];
                data->choices.back().v.offsetY += 20 * i;
                data->choices.back().up = i-1;
                data->choices.back().down = (i+1)%strings.size();
            }
            #endif
            break;
        }
        case 11: case 12:
        {
            #ifdef _USE_SAVE_
            std::vector<std::string> strings;
            data = new MenuData();
            *data = menu_database[ID];

            engine.saves.refreshSlotState();
            const std::vector<SlotState> states = engine.saves.getSlotState();
            for(size_t i = 0; i < states.size(); ++i)
            {
                if(states[i].used) strings.push_back("Save " + mlib::int2str(i+1) + ": " + states[i].header);
                else strings.push_back("Save " + mlib::int2str(i+1) + ": <empty>");
            }
            strings.push_back("back");

            data->visuals[0].size2 += (strings.size() - 1) * 20;
            data->visuals[1].size2 += (strings.size() - 1) * 20;

            data->choices[0].v.s = strings[0];
            data->choices[0].up = strings.size()-1;
            data->choices[0].down = 1;
            for(size_t i = 1; i < strings.size(); ++i)
            {
                data->choices.push_back(data->choices[0]);
                data->choices.back().v.s = strings[i];
                data->choices.back().v.offsetY += 20 * i;
                data->choices.back().up = i-1;
                data->choices.back().down = (i+1)%strings.size();
            }
            #endif
            break;
        }
        case 13:
        {
            #ifdef _USE_LANG_
            std::vector<std::string> strings;
            data = new MenuData();
            *data = menu_database[ID];

            for(uint32_t i = 0; i < engine.langs.getLangCount(); ++i)
            {
                strings.push_back(engine.langs.getLangName(i));
            }
            strings.push_back("back");

            data->visuals[0].size2 += (strings.size() - 1) * 20;
            data->visuals[1].size2 += (strings.size() - 1) * 20;

            data->choices[0].v.s = strings[0];
            data->choices[0].up = strings.size()-1;
            data->choices[0].down = 1;
            for(size_t i = 1; i < strings.size(); ++i)
            {
                data->choices.push_back(data->choices[0]);
                data->choices.back().v.s = strings[i];
                data->choices.back().v.offsetY += 20 * i;
                data->choices.back().up = i-1;
                data->choices.back().down = (i+1)%strings.size();
            }
            #endif
            break;
        }
    }
}

void Menu::validChoice(const std::string& sound)
{
    #ifdef _USE_SOUND_
    engine.sounds.play(sound);
    #endif
    switch(ID)
    {
        case 0:
            switch(cChoice)
            {
                case 0: engine.menus.delLastMenu(); return;
                case 1: engine.stop(); return;
                case 2: engine.menus.addMenu(1); return;
                #ifdef _USE_SAVE_
                case 3: engine.saves.setCurrentSlotHeader("TEST"); engine.saves.saveSlot(); return;
                #endif
            }
            return;
        case 1:
            switch(cChoice)
            {
                case 0: engine.menus.delLastMenu(); return;
            }
            return;
        case 2:
            switch(cChoice)
            {
                case 0: engine.menus.addMenu(11); return; // to new game
                case 1: engine.menus.addMenu(12); return; // to load game
                case 2: engine.menus.addMenu(3); return; // to settings
                case 3: engine.stop(); return;
            }
            return;
        case 3:
            switch(cChoice)
            {
                case 0: engine.menus.addMenu(4); engine.loadSetting(); return; // to general settings
                #ifdef _USE_INPUT_
                case 1: engine.menus.addMenu(9); return; // to key config
                #endif
                case 2: engine.menus.delLastMenu();return;
            }
            return;
        case 4:
            switch(cChoice)
            {
                case 0: engine.menus.addMenu(5); engine.menus.setLastMenuCurrentChoice(settWinS); return; // to window mode
                case 1: engine.menus.addMenu(6); engine.menus.setLastMenuCurrentChoice(engine.getCurrentVideoMode()); return; // to resolution
                case 2: engine.menus.addMenu(7);
                    switch(settFPS)
                    {
                        case 30: engine.menus.setLastMenuCurrentChoice(0); return;
                        case 60: engine.menus.setLastMenuCurrentChoice(1); return;
                        case 90: engine.menus.setLastMenuCurrentChoice(2); return;
                        case 120: engine.menus.setLastMenuCurrentChoice(3); return;
                        case 144: engine.menus.setLastMenuCurrentChoice(4); return;
                        case 160: engine.menus.setLastMenuCurrentChoice(5); return;
                        case 240: engine.menus.setLastMenuCurrentChoice(6); return;
                        case 300: engine.menus.setLastMenuCurrentChoice(7); return;
                        default: engine.menus.setLastMenuCurrentChoice(8); return;
                    }
                return; // to fps limit
                case 3: engine.menus.addMenu(8); engine.menus.setLastMenuCurrentChoice(settVol); return; // to master sound
                #ifdef _USE_LANG_
                case 4: engine.menus.addMenu(13); engine.menus.setLastMenuCurrentChoice(engine.langs.getCurrentLang()); return; // to language
                #endif
                case 5: engine.menus.delLastMenu(); engine.saveSetting(); return;
                case 6: engine.menus.delLastMenu(); engine.revertSetting(); return;
            }
            return;
        case 5:
            if(cChoice == 3)
            {
                engine.menus.delLastMenu();
            }
            else
            {
                engine.changeWindowMode(cChoice);
            }
            return;
        case 6:
            if(engine.getWindowMode() != 1 || cChoice == (int32_t)choices.size()-1)
            {
                engine.menus.delLastMenu();
            }
            else
            {
                engine.changeVideoMode(cChoice);
            }
            return;
        case 7:
            switch(cChoice)
            {
                case 0: settFPS = 30; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 1: settFPS = 60; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 2: settFPS = 90; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 3: settFPS = 120; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 4: settFPS = 144; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 5: settFPS = 160; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 6: settFPS = 240; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 7: settFPS = 300; engine.menus.delLastMenu(); engine.setFPSLimit(settFPS); return;
                case 8: engine.menus.delLastMenu(); return;
            }
            return;
        case 8:
            switch(cChoice)
            {
                case 0:
                    settVol += 5;
                    if(settVol > 100) settVol = 100;
                    choices[0].v->draw->setString("< " + mlib::int2str(settVol) + "% >");
                    #ifdef _USE_SOUND_
                    engine.sounds.play("so001.wav", settVol);
                    #endif
                    return;
                case 1: engine.menus.delLastMenu();  return;
            }
            return;
        case 9:
            if(cChoice == (int32_t)choices.size()-1)
            {
                engine.menus.delLastMenu();
            }
            else
            {
                engine.menus.delAndAdd(10);
                settChangingKey = true;
                settKeyCode = cChoice;
            }
            return;
        case 11:
            if(cChoice == (int32_t)choices.size()-1)
            {
                engine.menus.delLastMenu();
            }
            else
            {
                #ifdef _USE_SAVE_
                engine.saves.setSlot(cChoice);
                #endif
                resetGlobaleVariableAndString();
                engine.setMap("map000", 1);
                engine.menus.clear();
                gVar[GV_GAMESTATE] = 0; // dungeon state
                gVar[GV_MAPID] = 0; // map id
                engine.setScreenColor(sf::Color::Black, true);
            }
            return;
        case 12:
            if(cChoice == (int32_t)choices.size()-1)
            {
                engine.menus.delLastMenu();
            }
            else
            {
                #ifdef _USE_SAVE_
                if(engine.saves.loadSlot(cChoice))
                {
                    engine.saves.setSlot(cChoice);
                    engine.setMap("map000", 1); /// to change
                    engine.menus.clear();
                    gVar[GV_GAMESTATE] = 0; // dungeon state
                    gVar[GV_MAPID] = 0; // map id
                    engine.setScreenColor(sf::Color::Black, true);
                }
                #endif
            }
            return;
        case 13:
            if(cChoice != (int32_t)choices.size()-1)
            {
                #ifdef _USE_LANG_
                engine.langs.loadLang(cChoice);
                #endif
            }
            engine.menus.delLastMenu();
            return;
        default: return;
    }
}

void Menu::cancelChoice(const std::string& sound)
{
    #ifdef _USE_SOUND_
    engine.sounds.play(sound);
    #endif
    switch(ID)
    {
        case 0: return;
        case 8:
            switch(cChoice)
            {
                case 0:
                    settVol -= 5;
                    if(settVol < 0) settVol = 0;
                    choices[0].v->draw->setString("< " + mlib::int2str(settVol) + "% >");
                    #ifdef _USE_SOUND_
                    engine.sounds.play("so001.wav", settVol);
                    #endif
                    return;
            }
            return;
        case 9:
            if(cChoice < (int32_t)choices.size()-1)
            {
                // unbind
            }
            return;
        default: return;
    }
}

void Menu::applyCustomRules(MenuActor& ref, const int32_t &id)
{
    switch(ID) // menu id
    {
        case 0:
            #ifdef _USE_LANG_
            switch(id)
            {
                case 0:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TEST_MENU0") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TEST_MENU0"));
                    return;
                case 1:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TEST_MENU1") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TEST_MENU1"));
                    return;
                case 2:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TEST_MENU2") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TEST_MENU2"));
                    return;
            }
            #endif
            return;
        case 1:
            #ifdef _USE_LANG_
            switch(id)
            {
                case 0:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TEST_MENU3") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TEST_MENU3"));
                    return;
                case 1:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TEST_MENU4") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TEST_MENU4"));
                    return;
            }
            #endif
            return;
        case 2:
            #ifdef _USE_LANG_
            switch(id)
            {
                case 0:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TITLE0") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TITLE0"));
                    return;
                case 1:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TITLE1") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TITLE1"));
                    return;
                case 2:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TITLE2") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TITLE2"));
                    return;
                case 3:
                    if(ref.selected) ref.v->draw->setString("*" + engine.langs.getString("TITLE3") + "*");
                    else ref.v->draw->setString(engine.langs.getString("TITLE3"));
                    return;
            }
            #endif
            return;
        case 8:
            switch(id)
            {
                case 0:
                    ref.v->draw->setString("< " + mlib::int2str(settVol) + "% >");
                    return;
            }
            return;
        default: return;
    }
}
#endif
