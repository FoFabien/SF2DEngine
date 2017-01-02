#include "hud.hpp"
#include "../engine.hpp"
#include "variable.hpp"

HUD::HUD()
{
    showHUD = false;
    #ifdef DEBUG_BUILD
    #ifdef _USE_FONT_
    debug_text.setFont(*(engine.fonts.useCurrentFont()));
    #endif
    debug_text.setCharacterSize(10);
    #endif
}

HUD::~HUD()
{
    //dtor
}

void HUD::set(bool show)
{
    showHUD = show;
}

void HUD::update()
{
    #ifdef DEBUG_BUILD
    sf::IntRect rect = engine.getHUDBounds();
    debug_text.setPosition(rect.width-100, 5);
    #endif
}

void HUD::draw()
{
    if(showHUD)
    {

    }

    #ifdef DEBUG_BUILD
    engine.draw(&debug_text);
    #endif
}

#ifdef DEBUG_BUILD
void HUD::updateDebug()
{
    debug_text.setString(dStr[0]);
}
#endif
