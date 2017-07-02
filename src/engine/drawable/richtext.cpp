#include "richtext.hpp"
#include <SFML/Graphics.hpp>
#include <sstream>

static std::map<sf::String, sf::Color> colors;

// same as sf::Text styles
// define them locally to redefine them for a child class for example
#define Regular     0
#define Bold        1
#define Italic      2
#define Underlined  4

// TextChunk functions
TextChunk::TextChunk()
{
    style = Regular;
    color = sf::Color::White;
    endsInNewline = false;
}

void TextChunk::add(std::vector<TextChunk*>& chunks, TextChunk*& current, TextChunk* last)
{
    chunks.push_back(new TextChunk());
    current = chunks.back();

    if(chunks.size() > 2)
    {
        current->style = last->style;
        current->color = last->color;
    }
}

void TextChunk::format(TextChunk*& current, TextChunk* last, int32_t style)
{
    if((last->style & style) >= 0) current->style ^= style;
    else current->style |= style;
}

// RichText functions
RichText::RichText()
{
    nlratio = 1.5;
    csize = 20; // default parameters
    font = nullptr;
}

RichText::RichText(const sf::String& source, const sf::Font& font, uint32_t csize)
{
    nlratio = 1.5;
    this->csize = csize;
    this->font = &font;
    setString(source); // update the displayed string
}

RichText::~RichText()
{
    clear();
}

sf::String RichText::getString() const
{
    return string;
}

sf::String RichText::getSource() const
{
    return source;
}

void RichText::setString(const sf::String& source)
{
    this->source = source;
    if(!font) return; // nothing to display, return

    clear();
    if(source.getSize() == 0) return;

    chunks.push_back(new TextChunk());

    TextChunk* current = chunks[0];
    TextChunk* last = nullptr;
    bool escaped = false;

    // create the chunks
    for(uint32_t i = 0; i < source.getSize(); ++i)
    {
        last = current;
        if(escaped)
        {
            current->text += source[i];
            escaped = false;
            continue;
        }
        switch(source[i])
        {
            case '~': //	italics
            {
                TextChunk::add(chunks, current, last);
                TextChunk::format(current, last, Italic);
                current->color = last->color;
                break;
            }
            case '*': //	bold
            {
                TextChunk::add(chunks, current, last);
                TextChunk::format(current, last, Bold);
                current->color = last->color;
                break;
            }
            case '_': 	//	underline
            {
                TextChunk::add(chunks, current, last);
                TextChunk::format(current, last, Underlined);
                current->color = last->color;
                break;
            }
            case '#':	//	color
            {
                int32_t length = 0;
                int32_t start = i + 1;

                //	seek forward until the next whitespace
                while(!isspace(source[++i])) ++length;

                TextChunk::add(chunks, current, last);
                bool isValid;
                sf::Color c = getColor(source.toWideString().substr(start, length), isValid);
                if(isValid) current->color = c;
                break;

            }
            case '\\':	//	escape sequence for escaping formatting characters
            {
                if(escaped)
                {
                    current->text += source[i];
                    escaped = false;
                    break;
                }
                if (i + 1 < source.getSize())
                {
                    switch (source[i + 1])
                    {
                        case '~':
                        case '*':
                        case '_':
                        case '#':
                        {
                            escaped = true;
                            break;
                        }
                        default:
                            break;
                    }
                }
                if (!escaped) current->text += source[i];
                break;
            }
            case '\n':	// make a new chunk in the case of a newline
            {
                current->endsInNewline = true;
                TextChunk::add(chunks, current, last);
                break;
            }
            default:
            {
                escaped = false;
                current->text += source[i]; // append the character
                break;
            }
        }
    }
    build();
}

void RichText::build()
{
    float boundY = csize*nlratio;
    sf::Text* t = nullptr;
    sf::Vector2f partPos(0, 0);

    // still forced to clear here
    for(size_t i = 0; i < parts.size(); ++i)
        if(parts[i] != nullptr) delete parts[i];
    parts.clear();
    bounds.left = 0;
    bounds.top = 0;
    bounds.width = 0;
    bounds.height = 0;

    for(size_t i = 0; i < chunks.size(); ++i)
    {
        if(chunks[i]->text.getSize() != 0)
        {
            t = new sf::Text();
            t->setFillColor(chunks[i]->color);
            t->setString(chunks[i]->text);
            t->setStyle(chunks[i]->style);
            t->setCharacterSize(csize);

            if(font) t->setFont(*font);

            t->setPosition(partPos);
        }

        if(t) partPos.x += t->getLocalBounds().width;
        if(partPos.x >= bounds.width) bounds.width = partPos.x;
        if(chunks[i]->endsInNewline)
        {
            partPos.y += boundY;
            partPos.x = 0;
            bounds.height += boundY;
        }
        else if(i == chunks.size()-1)
        {
            bounds.height += boundY;
        }

        if(t)
        {
            parts.push_back((sf::Drawable*)t);
            t = nullptr;
        }
    }
}

void RichText::clear()
{
    for(size_t i = 0; i < chunks.size(); ++i)
        delete chunks[i];
    chunks.clear();
    for(size_t i = 0; i < parts.size(); ++i)
        if(parts[i] != nullptr) delete parts[i];
    parts.clear();
    bounds.left = 0;
    bounds.top = 0;
    bounds.width = 0;
    bounds.height = 0;
}

uint32_t RichText::getCharacterSize() const
{
    return csize;
}

void RichText::setCharacterSize(uint32_t size)
{
    csize = (size < 1) ? 1 : size;
    build();
}

float RichText::getNewLineSize() const
{
    return nlratio;
}

void RichText::setNewLineSize(float size)
{
    nlratio = (size <= 0) ? 1.5 : size;
    build();
}

const sf::Font* RichText::getFont() const
{
    return font;
}

void RichText::setFont(const sf::Font& font)
{
    this->font = &font;
    build();
}

void RichText::setFont(const sf::Font* font)
{
    this->font = font;
    build();
}

sf::FloatRect RichText::getLocalBounds() const
{
    return bounds;
}

sf::FloatRect RichText::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}

void RichText::addColor(const sf::String& name, const sf::Color& color)
{
    colors[name] = color;
}

void RichText::addColor(const sf::String& name, uint32_t argbHex)
{
    colors[name] = getColor(argbHex);
}

void RichText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    for(size_t i = 0; i < parts.size(); ++i)
        if(parts[i]) target.draw(*parts[i], states);
}

void RichText::initializeColors()
{
    colors["default"] = sf::Color::White;
    colors["black"] = sf::Color::Black;
    colors["blue"] = sf::Color::Blue;
    colors["cyan"] = sf::Color::Cyan;
    colors["green"] = sf::Color::Green;
    colors["magenta"] = sf::Color::Magenta;
    colors["red"] = sf::Color::Red;
    colors["white"] = sf::Color::White;
    colors["yellow"] = sf::Color::Yellow;
}

sf::Color RichText::getColor(const sf::String& source, bool& isValid)
{
    std::map<sf::String, sf::Color>::const_iterator result = colors.find(source);
    if(result == colors.end())
    {
        // basic hexadecimal check
        for(size_t i = 0; i < source.getSize(); ++i)
        {
            if((source[i] < 'a' || source [i] > 'f') && (source[i] < 'A' || source[i] > 'F') && (source[i] < '0' || source[i] > '9'))
            {
                isValid = false;
                return sf::Color::White;
            }
        }

        uint32_t hex = 0x0;
        if(!(std::istringstream(source) >> std::hex >> hex))
        {
            //	Error parsing; return default
            isValid = false;
            return sf::Color::White;
        }
        isValid = true;
        return getColor(hex);
    }
    isValid = true;
    return result->second;
}

sf::Color RichText::getColor(uint32_t argbHex)
{
    argbHex |= 0xff000000;
    return sf::Color(argbHex >> 16 & 0xFF, argbHex >> 8 & 0xFF, argbHex >> 0 & 0xFF, argbHex >> 24 & 0xFF);
}
