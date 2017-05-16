#ifndef HUD_HPP
#define HUD_HPP

#include "../engine/drawable/richtext.hpp"

class HUD
{
    public:
        HUD();
        ~HUD();
        // standard functions - don't remove
        void set(bool show);
        void update();
        void draw();
        #ifdef DEBUG_BUILD
        void updateDebug();
        #endif
    protected:
        bool showHUD;
        #ifdef DEBUG_BUILD
        RichText debug_text;   // text of the debug HUD
        #endif
};

#endif // HUD_HPP
