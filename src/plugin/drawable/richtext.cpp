#include "richtext.hpp"
#include <SFML/Graphics.hpp>
#include <sstream>

static std::map<sf::String, sf::Color> colors;

#define Regular     0
#define Bold        1
#define Italic      2
#define Underlined  4

namespace RT
{
    struct Chunk
    {
        sf::String text;
        int32_t style;
        sf::Color color;
        bool endsInNewline;
    };

    void newChunk(std::vector<Chunk*>& chunks, Chunk*& currentChunk, Chunk* lastChunk)
    {
        chunks.push_back(new Chunk());
        currentChunk = chunks.back();
        currentChunk->style = Regular;
        currentChunk->color = sf::Color::White;
        currentChunk->endsInNewline = false;

        if(chunks.size() > 2)
        {
            currentChunk->style = lastChunk->style;
            currentChunk->color = lastChunk->color;
        }
    }

    void processFormatting(Chunk* lastChunk, Chunk*& currentChunk, int32_t style)
    {
        if((lastChunk->style & style) >= 0) currentChunk->style ^= style;
        else currentChunk->style |= style;
    }
}

RichText::RichText()
{
    characterSize = 30;
    font = NULL;
}

RichText::RichText(const sf::String& source, const sf::Font& font, uint32_t characterSize)
{
    this->characterSize = characterSize;
    this->font = &font;
    setString(source);
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
    if(!font) return;

    clear();
    if(source.getSize() == 0) return;

    std::vector<RT::Chunk*> chunks;
    chunks.push_back(new RT::Chunk());

    RT::Chunk* currentChunk = chunks[0];
    currentChunk->style = Regular;
    currentChunk->color = sf::Color::White;
    currentChunk->endsInNewline = false;
    RT::Chunk* lastChunk = NULL;
    bool escaped = false;

    for(uint32_t i = 0; i < source.getSize(); ++i)
    {
        lastChunk = currentChunk;
        switch(source[i])
        {
            case '~': //	italics
            {
                if(escaped)
                {
                    currentChunk->text += source[i];
                    escaped = false;
                    break;
                }
                newChunk(chunks, currentChunk, lastChunk);
                processFormatting(lastChunk, currentChunk, Italic);
                currentChunk->color = lastChunk->color;
                break;
            }
            case '*': //	bold
            {
                if (escaped)
                {
                    currentChunk->text += source[i];
                    escaped = false;
                    break;
                }
                newChunk(chunks, currentChunk, lastChunk);
                processFormatting(lastChunk, currentChunk, Bold);
                currentChunk->color = lastChunk->color;
                break;
            }
            case '_': 	//	underline
            {
                if (escaped)
                {
                    currentChunk->text += source[i];
                    escaped = false;
                    break;
                }
                newChunk(chunks, currentChunk, lastChunk);
                processFormatting(lastChunk, currentChunk, Underlined);
                currentChunk->color = lastChunk->color;
                break;
            }
            case '#':	//	color
            {
                if (escaped)
                {
                    currentChunk->text += source[i];
                    escaped = false;
                    break;
                }

                int32_t length = 0;
                int32_t start = i + 1;

                //	seek forward until the next whitespace
                while(!isspace(source[++i])) ++length;

                newChunk(chunks, currentChunk, lastChunk);
                bool isValid;
                sf::Color c = getColor(source.toWideString().substr(start, length), isValid);
                if(isValid) currentChunk->color = c;
                break;

            }
            case '\\':	//	escape sequence for escaping formatting characters
            {
                if(escaped)
                {
                    currentChunk->text += source[i];
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
                if (!escaped) currentChunk->text += source[i];
                break;
            }
            case '\n':	// make a new chunk in the case of a newline
            {
                currentChunk->endsInNewline = true;
                newChunk(chunks, currentChunk, lastChunk);
                break;
            }
            default:
            {
                escaped = false;
                currentChunk->text += source[i];
                break;
            }
        }
    }

    sf::Text* t = NULL;
    sf::Vector2f partPos(0, 0);

    for(size_t i = 0; i < chunks.size(); ++i)
    {
        if(chunks[i]->text.getSize() != 0)
        {
            t = new sf::Text();
            t->setColor(chunks[i]->color);
            t->setString(chunks[i]->text);
            t->setStyle(chunks[i]->style);
            t->setCharacterSize(characterSize);

            if(font) t->setFont(*font);

            t->setPosition(partPos);
        }

        if(chunks[i]->endsInNewline)
        {
            partPos.y += characterSize*1.5;
            partPos.x = 0;
            bounds.height += characterSize*1.5;
        }
        else
        {
            if(t) partPos.x += t->getLocalBounds().width;
            if(partPos.x >= bounds.width) bounds.width = partPos.x;
            if(i == chunks.size()-1) bounds.height += characterSize*1.5;
        }

        if(t)
        {
            parts.push_back((sf::Drawable*)t);
            t = NULL;
        }
    }

    for(size_t i = 0; i < chunks.size(); ++i) delete chunks[i];
}

void RichText::clear()
{
    for(size_t i = 0; i < parts.size(); ++i)
        if(parts[i] != NULL) delete parts[i];
    parts.clear();
    bounds.left = 0;
    bounds.top = 0;
    bounds.width = 0;
    bounds.height = 0;
}

uint32_t RichText::getCharacterSize() const
{
    return characterSize;
}

void RichText::setCharacterSize(uint32_t size)
{
    characterSize = std::max(size, 1u);
    setString(source);
}

const sf::Font* RichText::getFont() const
{
    return font;
}

void RichText::setFont(const sf::Font& font)
{
    this->font = &font;
    setString(source);
}

void RichText::setFont(const sf::Font* font)
{
    this->font = font;
    setString(source);
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
        uint32_t hex = 0x0;
        if (!(std::istringstream(source) >> std::hex >> hex))
        {
            //	Error parsing; return default
            isValid = false;
            return sf::Color::White;
        };
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
