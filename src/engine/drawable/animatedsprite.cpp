#include "animatedsprite.hpp"
#include "../engine.hpp"

AnimatedSprite::AnimatedSprite()
{
    texture_name = "";
    a_size = sf::Vector2i(0, 0);
    a_currentLine = 0;
    a_currentFrame = 0;
    a_frameCount = 0;
    a_frameTime = sf::seconds(1);
    a_elapsedTime = sf::Time::Zero;
    running = false;
}

AnimatedSprite::AnimatedSprite(const AnimatedSprite &animatedsprite)
{
    texture_name = animatedsprite.texture_name;
    #ifdef _USE_TEXTURE_
    sf::Texture* t = engine.textures.use(texture_name);
    if(t) setTexture(*t);
    #endif

    a_size = animatedsprite.a_size;
    a_currentLine = animatedsprite.a_currentLine;
    a_currentFrame = animatedsprite.a_currentFrame;
    a_frameCount = animatedsprite.a_frameCount;
    a_frameTime = animatedsprite.a_frameTime;
    a_elapsedTime = animatedsprite.a_elapsedTime;
    running = animatedsprite.running;
}

AnimatedSprite::~AnimatedSprite()
{
    #ifdef _USE_TEXTURE_
    engine.textures.trash(texture_name);
    #endif
}

sf::FloatRect AnimatedSprite::getLocalBounds() const
{
    return sf::FloatRect(0, 0, a_size.x, a_size.y);
}

void AnimatedSprite::update()
{
    if(a_frameTime == sf::Time::Zero || a_frameCount == 0) // initialize the textureRect if it's the first time running
    {
        setTextureRect(sf::IntRect(0, 0, a_size.x, a_size.y));
        return;
    }
    if(running)
    {
        a_elapsedTime += engine.getElapsedTickTime(); // add the elapsed time
        while(a_elapsedTime >= a_frameTime) // compare with the targeted time
        {
            a_currentFrame = (a_currentFrame + 1) % a_frameCount; // set to the next frame
            a_elapsedTime -= a_frameTime; // substract the targeted time
        }
    }
    setTextureRect(sf::IntRect(a_currentFrame*a_size.x, a_currentLine*a_size.y, a_size.x, a_size.y)); // update the texture rect
}

void AnimatedSprite::setAnimationSize(const sf::Vector2i &size)
{
    a_size = size;
}

void AnimatedSprite::setFrameCount(const int32_t &n)
{
    a_frameCount = n;
}

void AnimatedSprite::setFrameSpeed(const sf::Time &t)
{
    a_frameTime = t;
}

void AnimatedSprite::setLine(const int32_t &l)
{
    if(a_currentLine != l) // reset the frame if the selected line is different from the previous one
    {
        a_currentFrame = 0;
        a_elapsedTime = sf::Time::Zero;
        a_currentLine = l;
    }

}

void AnimatedSprite::play()
{
    running = true;
}

void AnimatedSprite::pause()
{
    running = false;
}

void AnimatedSprite::stop()
{
    running = false;
    a_currentFrame = 0;
    a_elapsedTime = sf::Time::Zero;
}

bool AnimatedSprite::load(std::istream &f)
{
    char c;
    int32_t tmp32;
    texture_name.clear();
    do
    {
        if(!f.good()) return false;
        f.read(&c, 1);
        if(c != 0x00) texture_name += c;
    }while(c != 0x00);

    #ifdef _USE_TEXTURE_
    sf::Texture* texture = engine.textures.use(texture_name);
    if(texture) setTexture(*texture);
    #endif

    f.read((char*)&tmp32, 4);
    a_size.x = tmp32;
    f.read((char*)&tmp32, 4);
    a_size.y = tmp32;
    f.read((char*)&tmp32, 4);
    a_frameCount = tmp32;
    f.read((char*)&tmp32, 4);
    a_frameTime = sf::milliseconds(tmp32);
    a_elapsedTime = sf::Time::Zero;
    a_currentFrame = 0;
    a_currentLine = 0;
    setTextureRect(sf::IntRect(0, 0, a_size.x, a_size.y));
    return true;
}
