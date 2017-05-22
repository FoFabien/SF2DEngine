#ifndef _RICHTEXT_HPP_
#define _RICHTEXT_HPP_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <vector>
#include <stdint.h>

struct TextChunk
{
    sf::String text;
    int32_t style;
    sf::Color color;
    bool endsInNewline;

    TextChunk();
    static void add(std::vector<TextChunk*>& chunks, TextChunk*& current, TextChunk* last); // DON'T GIVE INVALID/EMPTY POINTER
    static void format(TextChunk*& current, TextChunk* last, int32_t style);
};

class RichText : public sf::Drawable, public sf::Transformable
{
    public:
        RichText();
        RichText(const sf::String& source, const sf::Font& font, uint32_t csize = 20);
        virtual ~RichText();

        virtual sf::String getString() const; // return the string
        virtual sf::String getSource() const; // return the source string
        virtual void setString(const sf::String& source); // set the string
        virtual void build(); // build the displayed string
        virtual void clear();

        virtual uint32_t getCharacterSize() const; // return the character size
        virtual void setCharacterSize(uint32_t size); // set the character size

        virtual float getNewLineSize() const; // return the ratio used to calculate a new line position
        virtual void setNewLineSize(float size); // set the ratio used to calculate a new line position

        virtual const sf::Font* getFont() const; // return the font used
        virtual void setFont(const sf::Font& font); // set the font
        virtual void setFont(const sf::Font* font);

        sf::FloatRect getLocalBounds() const;
        sf::FloatRect getGlobalBounds() const;

        // the values are shared between each instance
        static void initializeColors(); // MUST BE CALLED ONCE AT THE START
        static void addColor(const sf::String& name, const sf::Color& color);
        static void addColor(const sf::String& name, uint32_t argbHex);
        static sf::Color getColor(const sf::String& source, bool& isValid); // isValid is set to true if a valid color is returned, else false
        // note : source mustn't start with 0x if it's a hexadecimal code
        static sf::Color getColor(uint32_t argbHex);
    protected:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        std::vector<TextChunk*> chunks;
        std::vector<sf::Drawable*> parts;
        uint32_t csize;
        float nlratio;
        const sf::Font* font;

        sf::String source;
        sf::String string;
        sf::FloatRect bounds;
};

#endif // _RICHTEXT_HPP_
