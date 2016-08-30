#ifndef ANIMATEDSPRITE_HPP
#define ANIMATEDSPRITE_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "../../mlib/mload.hpp"
#include <stdint.h>

class AnimatedSprite: public sf::Sprite, public MLoad
{
    public:
        AnimatedSprite();
        AnimatedSprite(const AnimatedSprite &animatedsprite);
        ~AnimatedSprite();
        void update(); /// update the animation frame
        sf::FloatRect getLocalBounds() const; /// return local bounds - note: to be improved ?
        void setAnimationSize(const sf::Vector2i &size); /// set the subrect size
        void setFrameCount(const int32_t &n); /// set the animation number of frame
        void setFrameSpeed(const sf::Time &t); /// set the animation speed (milliseconds by frame)
        void setLine(const int32_t &l); /// set the current line
        void play();
        void pause();
        void stop(); /// pause the animation and reset the frame
    protected:
        bool load(std::istream &f); // load the file from the memory or the drive
        sf::Vector2i a_size;
        int32_t a_currentLine;
        int32_t a_currentFrame;
        int32_t a_frameCount;
        sf::Time a_frameTime;
        sf::Time a_elapsedTime;
        std::string texture_name;
        bool running;
};

#endif // ANIMATEDSPRITE_HPP
