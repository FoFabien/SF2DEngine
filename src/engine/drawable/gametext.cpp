#include "gametext.hpp"
#include "../engine.hpp"
#include "../mlib/mlib.hpp"

#define Regular     0
#define Bold        1
#define Italic      2
#define Underlined  4
#define Image       8

namespace GT
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

GameText::GameText()
{
    characterSize = 30;
    font = NULL;

    a_frameTime = sf::seconds(1);
    a_elapsedTime = sf::Time::Zero;
    running = false;
    currentPos = 0;
    letter_sound = "";
    complete_sound = "";
}

GameText::GameText(const sf::String& source, const sf::Font& font, uint32_t characterSize)
{
    this->characterSize = characterSize;
    this->font = &font;
    setString(source);

    a_frameTime = sf::Time::Zero;
    a_elapsedTime = sf::Time::Zero;
    running = false;
    currentPos = 0;
    letter_sound = "";
    complete_sound = "";
}

GameText::~GameText()
{
    clear();
}

void GameText::setString(const sf::String& source, const bool& updateSource)
{
    if(updateSource)
    {
        this->source = source;
        currentPos = 0;
        a_elapsedTime = sf::Time::Zero;
    }

    if(!font) return;

    clear();
	if(source.getSize() == 0 || updateSource) return;
    std::vector<GT::Chunk*> chunks;
    chunks.push_back(new GT::Chunk());

    GT::Chunk* currentChunk = chunks[0];
    currentChunk->style = Regular;
    currentChunk->color = sf::Color::White;
    currentChunk->endsInNewline = false;
    GT::Chunk* lastChunk = NULL;
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
                sf::String substr = source.toWideString().substr(start, length);
                sf::Color c = getColor(substr, isValid);
                if(isValid) currentChunk->color = c;
                else
                {
                    currentChunk->style = Image;
                    currentChunk->text = substr.toAnsiString();
                    newChunk(chunks, currentChunk, lastChunk);
                }
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

    MDrawable* dra = NULL;
    sf::Text* t = NULL;
    sf::Vector2f partPos(0, 0);

    for(size_t i = 0; i < chunks.size(); ++i)
    {
        switch(chunks[i]->style)
        {
            case Image:
            {
                dra = new MDrawable();
                if(dra->loadFromMemory(chunks[i]->text))
                {
                    dra->setPosition(partPos);
                    partPos.x += dra->getLocalBounds().width;
                    drawables.push_back(dra);
                    parts.push_back(dra->getDrawable());
                }
                break;
            }
            default:
            {
                dra = new MDrawable();
                if(dra->loadFromData(STDTEXT, ""))
                {
                    if(chunks[i]->text.getSize() != 0)
                    {
                        t = (sf::Text*)dra->getDrawable();
                        t->setFillColor(chunks[i]->color);
                        t->setString(chunks[i]->text);
                        t->setStyle(chunks[i]->style);
                        t->setCharacterSize(characterSize);

                        if(font) t->setFont(*font);

                        t->setPosition(partPos);
                    }
                    else delete dra;

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
                        drawables.push_back(dra);
                        parts.push_back((sf::Drawable*)t);
                        t = NULL;
                    }
                }
                break;
            }
        }
    }

    for(size_t i = 0; i < chunks.size(); ++i) delete chunks[i];
}

void GameText::clear()
{
    for(MDrawable* &i: drawables) if(i) delete i;
    drawables.clear();
    parts.clear();
    bounds.left = 0;
    bounds.top = 0;
    bounds.width = 0;
    bounds.height = 0;
}

void GameText::update()
{
    if(running)
    {
        if(currentPos >= (int32_t)source.getSize()-1) return;

        a_elapsedTime += engine.getElapsedTickTime();
        while(a_elapsedTime >= a_frameTime && running)
        {
            bool finished = false;
            bool escaped = false;
            while(!finished && currentPos + 1 < (int32_t)source.getSize())
            {
                currentPos++;
                switch(source[currentPos])
                {
                    case '~': case '*': case '_':
                    {

                        if(escaped) finished = true;
                        break;
                    }
                    case '#':
                    {
                        if(!escaped)
                        {
                            while(!isspace(source[currentPos]) && currentPos < (int32_t)source.getSize()) ++currentPos;
                        }
                        break;
                    }
                    case '\\':
                    {
                        if(escaped)
                        {
                            finished = true;
                            break;
                        }
                        if (currentPos + 1 < (int32_t)source.getSize())
                        {
                            switch (source[currentPos + 1])
                            {
                                case '~': case '*': case '_': case '#':
                                {
                                    escaped = true;
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        if (!escaped) finished = true;
                        break;
                    }
                    case ' ':
                        break;
                    default:
                        finished = true;
                        break;
                }
            }

            a_elapsedTime -= a_frameTime;
            if(currentPos >= (int32_t)source.getSize()-1)
            {
                a_elapsedTime = sf::Time::Zero; // end of string, stop
                #ifdef _USE_SOUND_
                engine.sounds.play(complete_sound);
                #endif
            }
            #ifdef _USE_SOUND_
            else engine.sounds.play(letter_sound);
            #endif

            if(a_elapsedTime < a_frameTime) setString(mlib::sfsubstr(source, 0, currentPos+1), false); // update the string if processing is done
        }
    }
}

void GameText::play()
{
    running = true;
}

void GameText::pause()
{
    running = false;
}

void GameText::stop()
{
    running = false;
    currentPos = 0;
}

void GameText::setFrameSpeed(const sf::Time &t)
{
    a_frameTime = t;
}

void GameText::showFullText()
{
    #ifdef _USE_SOUND_
    if(currentPos < (int32_t)source.getSize()-1 && running) engine.sounds.play(complete_sound);
    #endif
    currentPos = source.getSize()-1;
    setString(source, false);
}

void GameText::setLetterSound(const std::string& file)
{
    letter_sound = file;
}

void GameText::setCompleteSound(const std::string& file)
{
    complete_sound = file;
}
