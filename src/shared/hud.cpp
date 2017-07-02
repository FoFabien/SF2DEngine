#include "hud.hpp"
#include "../engine/engine.hpp"
#include "variable.hpp"

HUD::HUD()
{
    showHUD = false;
    #ifdef DEBUG_BUILD
    #ifdef _USE_FONT_
    debug_text.setFont(*(engine.fonts.useCurrentFont()));
    #endif
    debug_text.setCharacterSize(10);
    debug_input.setSize(sf::Vector2f(4, 4));
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
    for(int8_t i = 0; i < KCOUNT; ++i)
    {
        if(engine.input.isPressedNow(i)) debug_input.setFillColor(sf::Color::Yellow);
        else if(engine.input.isPressed(i)) debug_input.setFillColor(sf::Color::Red);
        else if(engine.input.isReleasedNow(i)) debug_input.setFillColor(sf::Color::Green);
        else if(engine.input.isReleased(i)) debug_input.setFillColor(sf::Color::Blue);
        else debug_input.setFillColor(sf::Color::Magenta); // error case
        debug_input.setPosition(1+5*i, 1);
        engine.draw(&debug_input);
    }
    #endif
}

#ifdef DEBUG_BUILD
void HUD::updateDebug()
{
    debug_text.setString(dStr[0]);
}
#endif
