#ifndef MANAGERMENU_HPP
#define MANAGERMENU_HPP

#include "../game/menu.hpp"

#ifdef _USE_MENU_
class ManagerMenu
{
    public:
        ManagerMenu();
        ~ManagerMenu();
        void clear();
        bool hasMenu() const;
        void update();
        void draw();
        bool addMenu(int32_t id); /// add a new menu to the stack
        void delLastMenu(); /// delete the last menu added to the stack
        void delAndAdd(int32_t id); /// delete the last menu and add a new one
        bool gamePaused() const;
        void setLastMenuCurrentChoice(int32_t n);
        void enableInput();
        void disableInput();
    protected:
        std::vector<Menu*> menus;
        bool delLast;
        bool cleared;
        int32_t pause_index;
        bool reset_index; // to end the pause at the next tick
        uint32_t menu_count; // number of active menu
        int32_t menu_queue;
        bool input;
};
#endif

#endif // MANAGERMENU_HPP
