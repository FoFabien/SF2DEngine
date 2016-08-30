#include "mdrawable.hpp"
#include "../../engine.hpp"
#include "richtext.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "animatedsprite.hpp"
#include "gametext.hpp"

MDrawable::MDrawable()
{
    data = nullptr;
    type = UNDEF_DRAW;
    string = "";
}

MDrawable::MDrawable(MDrawable &mdrawable)
{
    type = mdrawable.type;
    string = mdrawable.string;
    data = nullptr;
    switch(type)
    {
        case UNDEF_DRAW: break;
        case TEXT:
        {
            data = (sf::Drawable*)new RichText();
            #ifdef _USE_FONT_
            const sf::Font* font = engine.fonts.useCurrentFont();
            if(font)
            {
                ((RichText*)data)->setFont(font);
            }
            #endif
            ((RichText*)data)->setString(string);
            break;
        }
        case SPRITE:
        {
            data = (sf::Drawable*)new sf::Sprite();
            #ifdef _USE_TEXTURE_
            sf::Texture* texture = engine.textures.use(string);
            if(texture) ((sf::Sprite*)data)->setTexture(*texture);
            #endif
            break;
        }
        case ANIMATION:
        {
            data = (sf::Drawable*)new AnimatedSprite(*((AnimatedSprite*)mdrawable.data));
            break;
        }
        case RECTANGLE:
        {
            data = (sf::Drawable*)new sf::RectangleShape(*((sf::RectangleShape*)mdrawable.data));
            break;
        }
        case CIRCLE:
        {
            data = (sf::Drawable*)new sf::CircleShape(*((sf::CircleShape*)mdrawable.data));
            break;
        }
        case STDTEXT:
        {
            data = (sf::Drawable*)new sf::Text();
            #ifdef _USE_FONT_
            const sf::Font* font = engine.fonts.useCurrentFont();
            if(font)
            {
                ((sf::Text*)data)->setFont(*font);
            }
            #endif
            ((sf::Text*)data)->setString(string);
            break;
        }
        case GAMETEXT:
        {
            data = (sf::Drawable*)new GameText();
            #ifdef _USE_FONT_
            const sf::Font* font = engine.fonts.useCurrentFont();
            if(font)
            {
                ((GameText*)data)->setFont(font);
            }
            #endif
            ((GameText*)data)->setString(string);
            break;
        }
    }
}

MDrawable::~MDrawable()
{
    clear();
}

void MDrawable::clear()
{
    switch(type)
    {
        case UNDEF_DRAW: break;
        case TEXT:
            #ifdef _USE_FONT_
            engine.fonts.trash(((RichText*)(data))->getFont());
            #endif
            delete (RichText*)(data);
            break;
        case SPRITE:
            #ifdef _USE_TEXTURE_
            engine.textures.trash(((sf::Sprite*)(data))->getTexture());
            #endif
            delete (sf::Sprite*)(data);
            break;
        case ANIMATION:
            delete (AnimatedSprite*)(data);
            break;
        case RECTANGLE: delete (sf::RectangleShape*)(data); break;
        case CIRCLE: delete (sf::CircleShape*)(data); break;
        case STDTEXT:
            #ifdef _USE_FONT_
            engine.fonts.trash(((sf::Text*)(data))->getFont());
            #endif
            delete (sf::Text*)(data);
            break;
        case GAMETEXT:
            #ifdef _USE_FONT_
            engine.fonts.trash(((GameText*)(data))->getFont());
            #endif
            delete (GameText*)(data);
            break;
    }
    data = nullptr;
    type = UNDEF_DRAW;
    string = "";
}

void MDrawable::update()
{
    if(!data) return;
    switch(type)
    {
        default: return;
        case ANIMATION:
            ((AnimatedSprite*)data)->update();
            return;
        case GAMETEXT: ((GameText*)data)->update(); return;
    }
}

void MDrawable::draw()
{
    if(!data) return;
    switch(type)
    {
        case UNDEF_DRAW: return;
        case TEXT: engine.draw((RichText*)data); return;
        case SPRITE: engine.draw((sf::Sprite*)data); return;
        case ANIMATION:
            engine.draw((AnimatedSprite*)data);
            return;
        case RECTANGLE: engine.draw((sf::RectangleShape*)data); return;
        case CIRCLE: engine.draw((sf::CircleShape*)data); return;
        case STDTEXT: engine.draw((sf::Text*)data); return;
        case GAMETEXT: engine.draw((GameText*)data); return;
    }
}

sf::FloatRect MDrawable::getLocalBounds() const
{
    if(!data) return sf::FloatRect(0, 0, 0, 0);
    switch(type)
    {
        case UNDEF_DRAW: return sf::FloatRect(0, 0, 0, 0);
        case TEXT: return ((RichText*)data)->getLocalBounds();
        case SPRITE: return ((sf::Sprite*)data)->getLocalBounds();
        case ANIMATION: return ((AnimatedSprite*)data)->getLocalBounds();
        case RECTANGLE: return ((sf::RectangleShape*)data)->getLocalBounds();
        case CIRCLE: return ((sf::CircleShape*)data)->getLocalBounds();
        case STDTEXT: return ((sf::Text*)data)->getLocalBounds();
        case GAMETEXT: return ((GameText*)data)->getLocalBounds();
    }
    return sf::FloatRect(0, 0, 0, 0);
}

MDrawable_type MDrawable::getType() const
{
    return type;
}

void MDrawable::setColor(const sf::Color &c)
{
    if(!data) return;
    switch(type)
    {
        case UNDEF_DRAW: return;
        case TEXT: return;
        case SPRITE: ((sf::Sprite*)data)->setColor(c); return;
        case ANIMATION: ((AnimatedSprite*)data)->setColor(c); return;
        case RECTANGLE: ((sf::RectangleShape*)data)->setFillColor(c); return;
        case CIRCLE: ((sf::CircleShape*)data)->setFillColor(c); return;
        case STDTEXT: ((sf::Text*)data)->setColor(c); return;
        case GAMETEXT: return;
    }
}

void MDrawable::setPosition(const sf::Vector2i &p)
{
    setPosition(p.x, p.y);
}

void MDrawable::setPosition(const int &x, const int &y)
{
    if(!data) return;
    switch(type)
    {
        case UNDEF_DRAW: return;
        case TEXT: ((RichText*)data)->setPosition(x, y); return;
        case SPRITE: ((sf::Sprite*)data)->setPosition(x, y); return;
        case ANIMATION: ((AnimatedSprite*)data)->setPosition(x, y); return;
        case RECTANGLE: ((sf::RectangleShape*)data)->setPosition(x, y); return;
        case CIRCLE: ((sf::CircleShape*)data)->setPosition(x, y); return;
        case STDTEXT: ((sf::Text*)data)->setPosition(x, y); return;
        case GAMETEXT: ((GameText*)data)->setPosition(x, y); return;
    }
}

void MDrawable::setPosition(const sf::Vector2f &p)
{
    if(!data) return;
    switch(type)
    {
        case UNDEF_DRAW: return;
        case TEXT: ((RichText*)data)->setPosition(p); return;
        case SPRITE: ((sf::Sprite*)data)->setPosition(p); return;
        case ANIMATION: ((AnimatedSprite*)data)->setPosition(p); return;
        case RECTANGLE: ((sf::RectangleShape*)data)->setPosition(p); return;
        case CIRCLE: ((sf::CircleShape*)data)->setPosition(p); return;
        case STDTEXT: ((sf::Text*)data)->setPosition(p); return;
        case GAMETEXT: ((GameText*)data)->setPosition(p); return;
    }
}

sf::Vector2f MDrawable::getPosition()
{
    if(!data) return sf::Vector2f(0, 0);
    switch(type)
    {
        case UNDEF_DRAW: return sf::Vector2f(0, 0);
        case TEXT: return ((RichText*)data)->getPosition();
        case SPRITE: return ((sf::Sprite*)data)->getPosition();
        case ANIMATION: return ((AnimatedSprite*)data)->getPosition();
        case RECTANGLE: return ((sf::RectangleShape*)data)->getPosition();
        case CIRCLE: return ((sf::CircleShape*)data)->getPosition();
        case STDTEXT: return ((sf::Text*)data)->getPosition();
        case GAMETEXT: return ((GameText*)data)->getPosition();
    }
    return sf::Vector2f(0, 0);
}

bool MDrawable::loadFromData(MDrawable_type t, std::string str)
{
    clear();
    type = t;
    switch(type)
    {
        case UNDEF_DRAW: break;
        case TEXT:
        {
            data = (sf::Drawable*)new RichText();
            #ifdef _USE_FONT_
            const sf::Font* font = engine.fonts.useCurrentFont();
            if(font)
            {
                ((RichText*)data)->setFont(font);
            }
            #endif
            ((RichText*)data)->setString(str);
            break;
        }
        case SPRITE:
        {
            data = (sf::Drawable*)new sf::Sprite();
            #ifdef _USE_TEXTURE_
            sf::Texture* texture = engine.textures.use(str);
            if(texture) ((sf::Sprite*)data)->setTexture(*texture);
            #endif
            break;
        }
        case ANIMATION:
        {
            data = (sf::Drawable*)new AnimatedSprite();
            ((AnimatedSprite*)data)->loadFromMemory(str);
            break;
        }
        case RECTANGLE:
        {
            data = (sf::Drawable*)new sf::RectangleShape();
            break;
        }
        case CIRCLE:
        {
            data = (sf::Drawable*)new sf::CircleShape();
            break;
        }
        case STDTEXT:
        {
            data = (sf::Drawable*)new sf::Text();
            #ifdef _USE_FONT_
            const sf::Font* font = engine.fonts.useCurrentFont();
            if(font)
            {
                ((sf::Text*)data)->setFont(*font);
            }
            #endif
            ((sf::Text*)data)->setString(str);
            break;
        }
        case GAMETEXT:
        {
            data = (sf::Drawable*)new GameText();
            #ifdef _USE_FONT_
            const sf::Font* font = engine.fonts.useCurrentFont();
            if(font)
            {
                ((GameText*)data)->setFont(font);
            }
            #endif
            ((GameText*)data)->setString(str);
            break;
        }
    }
    string = str;
    return true;
}

void MDrawable::play()
{
    if(!data) return;
    switch(type)
    {
        default: return;
        case ANIMATION: ((AnimatedSprite*)data)->play(); return;
        case GAMETEXT: ((GameText*)data)->play(); return;
    }
}

void MDrawable::pause()
{
    if(!data) return;
    switch(type)
    {
        default: return;
        case ANIMATION: ((AnimatedSprite*)data)->pause(); return;
        case GAMETEXT: ((GameText*)data)->pause(); return;
    }
}

void MDrawable::stop()
{
    if(!data) return;
    switch(type)
    {
        default: return;
        case ANIMATION: ((AnimatedSprite*)data)->stop(); return;
        case GAMETEXT: ((GameText*)data)->stop(); return;
    }
}

void MDrawable::setFrameSpeed(const sf::Time &t)
{
    if(!data) return;
    switch(type)
    {
        default: return;
        case ANIMATION: ((AnimatedSprite*)data)->setFrameSpeed(t); return;
        case GAMETEXT: ((GameText*)data)->setFrameSpeed(t); return;
    }
}

void MDrawable::setLine(const int32_t &l)
{
    if(!data) return;
    switch(type)
    {
        default: return;
        case ANIMATION: ((AnimatedSprite*)data)->setLine(l); return;
    }
}

void MDrawable::setString(const sf::String &str)
{
    if(!data) return;
    switch(type)
    {
        case TEXT: ((RichText*)data)->setString(str); return;
        case STDTEXT: ((sf::Text*)data)->setString(str); return;
        case GAMETEXT: ((GameText*)data)->setString(str); return;
        default: return;
    }
}

void MDrawable::setCharacterSize(const int32_t &s)
{
    if(!data) return;
    switch(type)
    {
        case TEXT: ((RichText*)data)->setCharacterSize(s); return;
        case STDTEXT: ((sf::Text*)data)->setCharacterSize(s); return;
        case GAMETEXT: ((GameText*)data)->setCharacterSize(s); return;
        default: return;
    }
}

void MDrawable::setFont(const sf::Font& font)
{
    if(!data) return;
    switch(type)
    {
        case TEXT:
            #ifdef _USE_FONT_
            engine.fonts.trash(((RichText*)(data))->getFont());
            #endif // _USE_FONT_
            ((RichText*)data)->setFont(font);
            return;
        case STDTEXT:
            #ifdef _USE_FONT_
            engine.fonts.trash(((sf::Text*)(data))->getFont());
            #endif // _USE_FONT_
            ((sf::Text*)data)->setFont(font);
            return;
        case GAMETEXT:
            #ifdef _USE_FONT_
            engine.fonts.trash(((GameText*)(data))->getFont());
            #endif // _USE_FONT_
            ((GameText*)data)->setFont(font);
            return;
        default: return;
    }
}

void MDrawable::setSize(const int &x, const int &y)
{
    if(!data) return;
    switch(type)
    {
        case RECTANGLE: ((sf::RectangleShape*)data)->setSize(sf::Vector2f(x, y)); return;
        case CIRCLE:
            ((sf::CircleShape*)data)->setRadius(x);
            ((sf::CircleShape*)data)->setPointCount(y);
            return;
        default: return;
    }
}

void MDrawable::setSize(const sf::Vector2i &s)
{
    setSize(s.x, s.y);
}

void MDrawable::setSize(const int &radius)
{
    if(!data) return;
    switch(type)
    {
        case CIRCLE: ((sf::CircleShape*)data)->setRadius(radius); return;
        default: return;
    }
}

sf::Drawable* MDrawable::getDrawable()
{
    return data;
}

void MDrawable::setSoundFile1(const std::string& str)
{
    if(!data) return;
    switch(type)
    {
        case GAMETEXT: ((GameText*)data)->setLetterSound(str); return;
        default: return;
    }
}

void MDrawable::setSoundFile2(const std::string& str)
{
    if(!data) return;
    switch(type)
    {
        case GAMETEXT: ((GameText*)data)->setCompleteSound(str); return;
        default: return;
    }
}

/*Note:
UNDEF_DRAW=0
TEXT=1
SPRITE=2
ANIMATION=3*/

bool MDrawable::load(std::istream &f)
{
    char c;
    f.read(&c, 1);
    MDrawable_type t = static_cast<MDrawable_type>(c);
    std::string str;
    switch(t)
    {
        case UNDEF_DRAW: clear(); return true;
        default:
            do
            {
                if(!f.good()) return false;
                f.read(&c, 1);
                if(c != 0x00) str += c;
            }while(c != 0x00);
    }
    return loadFromData(t, str);
}
