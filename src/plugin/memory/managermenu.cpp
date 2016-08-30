#include "managermenu.hpp"
#ifdef _USE_MENU_
#include "../../engine.hpp"

ManagerMenu::ManagerMenu()
{
    delLast = false;
    cleared = false;
    pause_index = -1;
    reset_index = false;
    menu_count = 0;
    menu_queue = -1;
    input = true;
}

ManagerMenu::~ManagerMenu()
{
    for(Menu* &i : menus) delete i;
}

void ManagerMenu::clear()
{
    for(Menu* &i : menus) delete i;
    menus.clear();
    delLast = false;
    cleared = true;
    pause_index = -1;
    reset_index = false;
    menu_count = 0;
    menu_queue = -1;
}

bool ManagerMenu::hasMenu() const
{
    return (!menus.empty());
}

void ManagerMenu::update()
{
    cleared = false;
    if(reset_index)
    {
        pause_index = -1;
        reset_index = false;
    }

    if(delLast && !menus.empty())
    {
        delete menus[menus.size()-1];
        menus.pop_back();
        if(pause_index == (int32_t)menus.size()) reset_index = true;
        delLast = false;
        #ifdef _USE_INPUT_
        //if(menus.empty()) engine.input.loadState();
        #endif
    }

    while(menu_queue != -1)
    {
        addMenu(menu_queue);
        menu_queue = -1;
    }

    menu_count = menus.size();
    if(menus.empty()) return;

    if(input)
    {
        uint32_t i = menu_count - 1;
        menus[i]->action();
        if(cleared) return;
    }

    for(uint32_t i = 0; i < menu_count; ++i) menus[i]->update();
}

void ManagerMenu::draw()
{
    for(uint32_t i = 0; i < menu_count; ++i) menus[i]->draw();
}

bool ManagerMenu::addMenu(int32_t id)
{
    Menu* ptr = new Menu();
    ptr->init(id);
    if(ptr->isInitialized())
    {
        #ifdef _USE_INPUT_
        //if(menus.empty()) engine.input.saveState();
        #endif
        if(ptr->isPausingGame() && pause_index == -1) pause_index = menus.size();
        menus.push_back(ptr);
        return true;
    }
    delete ptr;
    return false;
}

void ManagerMenu::delLastMenu()
{
    delLast = true;
}

void ManagerMenu::delAndAdd(int32_t id)
{
    delLast = true;
    if(menu_queue == -1) menu_queue = id;
}

bool ManagerMenu::gamePaused() const
{
    return (pause_index != -1);
}

void ManagerMenu::setLastMenuCurrentChoice(int32_t n)
{
    if(menus.empty()) return;
    menus.back()->setCurrentChoice(n);
}

void ManagerMenu::enableInput()
{
    input = true;
}

void ManagerMenu::disableInput()
{
    input = false;
}
#endif
