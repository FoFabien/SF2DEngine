#ifndef _GAMETEXT_HPP_
#define _GAMETEXT_HPP_

#include "richtext.hpp"
#include <SFML/System/Time.hpp>
#include "mdrawable.hpp"

// this class is for my game engine

class GameText: public RichText
{
    public:
        GameText();
        GameText(const sf::String& source, const sf::Font& font, uint32_t characterSize = 20);
        ~GameText();

        void setString(const sf::String& source, const bool& updateSource = true); /// updateSource must be true to modify the source string
        void build();
        void clear(); /// clear the string data. don't clear the animation data

        // animation
        void update(); /// update the animation
        void play(); /// play the animation
        void pause(); /// pause the animation
        void stop(); /// stop the animation
        void setFrameSpeed(const sf::Time &t); /// set the speed of the animation (time by letter)
        void showFullText(); /// complete the animation instantly
        void setLetterSound(const std::string& file); /// set the sound played each tile a letter is added during the animation
        void setCompleteSound(const std::string& file); /// set the sound player when the animation is completed
    protected:
        // animation
        sf::Time a_frameTime;
        sf::Time a_elapsedTime;
        bool running;
        int32_t currentPos;
        std::string letter_sound;
        std::string complete_sound;
        std::vector<MDrawable*> drawables;
};

#endif // _GAMETEXT_HPP_
