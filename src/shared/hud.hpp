#ifndef HUD_HPP
#define HUD_HPP

#include "../engine/drawable/richtext.hpp"
#ifdef DEBUG_BUILD
#include <SFML/Graphics/RectangleShape.hpp>
#endif

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
        sf::RectangleShape debug_input;
        #endif
};

#endif // HUD_HPP
