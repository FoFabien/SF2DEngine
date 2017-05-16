#ifndef MDRAWABLE_HPP
#define MDRAWABLE_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/String.hpp>

#include <string>
#include <stdint.h>
#include "../mlib/mload.hpp"

enum MDrawable_type
{
    UNDEF_DRAW, TEXT, SPRITE, ANIMATION, RECTANGLE, CIRCLE, STDTEXT, GAMETEXT
};

class MDrawable: public MLoad
{
    public:
        MDrawable();
        MDrawable(MDrawable &mdrawable);
        ~MDrawable();
        // standard function
        void clear();
        void update();
        void draw();
        sf::FloatRect getLocalBounds() const;
        MDrawable_type getType() const;
        void setColor(const sf::Color &c);
        void setPosition(const sf::Vector2i &p);
        void setPosition(const int &x, const int &y);
        void setPosition(const sf::Vector2f &p);
        sf::Vector2f getPosition();
        bool loadFromData(MDrawable_type t, std::string str);
        sf::Drawable* getDrawable();
        void setSize(const int& x, const int& y);
        void setSize(const sf::Vector2i& s);
        void setSize(const int &radius);

        // animated class
        void play();
        void pause();
        void stop();
        void setFrameSpeed(const sf::Time &t);
        void setLine(const int32_t &l);
        void setString(const sf::String &str);

        // text class
        void setCharacterSize(const int32_t &s);
        void setFont(const sf::Font& font);

        // others
        void setSoundFile1(const std::string& str);
        void setSoundFile2(const std::string& str);
    protected:
        bool load(std::istream &f);
        MDrawable_type type;
        sf::Drawable *data;
        std::string string;
};

#endif // MDRAWABLE_HPP
