#ifndef _RICHTEXT_HPP_
#define _RICHTEXT_HPP_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <vector>
#include <stdint.h>

class RichText : public sf::Drawable, public sf::Transformable
{
    public:
        RichText();
        RichText(const sf::String& source, const sf::Font& font, uint32_t characterSize = 20);
        virtual ~RichText();

        sf::String getString() const; // return the string
        sf::String getSource() const; // return the source string
        void setString(const sf::String& source); // set the string
        void clear();

        uint32_t getCharacterSize() const; // return the character size
        void setCharacterSize(uint32_t size); // set the character size

        const sf::Font* getFont() const; // return the font used
        void setFont(const sf::Font& font); // set the font
        void setFont(const sf::Font* font);

        sf::FloatRect getLocalBounds() const;
        sf::FloatRect getGlobalBounds() const;

        //	Set names for color substitutions (for example, ff0000 would be substituted for "red")
        // the values are shared between each instance
        static void initializeColors();
        static void addColor(const sf::String& name, const sf::Color& color);
        static void addColor(const sf::String& name, uint32_t argbHex);
        static sf::Color getColor(const sf::String& source, bool& isValid); // isValid is set to true if a valid color is returned, else false
        static sf::Color getColor(uint32_t argbHex);
    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        std::vector<sf::Drawable*> parts;
        uint32_t characterSize;
        const sf::Font* font;

        sf::String source;
        sf::String string;
        sf::FloatRect bounds;
};

#endif // _RICHTEXT_HPP_
