#include "gametext.hpp"
#include "../engine.hpp"
#include "../mlib/mlib.hpp"

#define Regular     0
#define Bold        1
#define Italic      2
#define Underlined  4
#define Image       8

GameText::GameText()
{
    nlratio = 1.5;
    csize = 30;
    font = nullptr;

    a_frameTime = sf::seconds(1);
    a_elapsedTime = sf::Time::Zero;
    running = false;
    currentPos = 0;
    letter_sound = "";
    complete_sound = "";
}

GameText::GameText(const sf::String& source, const sf::Font& font, uint32_t csize)
{
    nlratio = 1.5;
    this->csize = csize;
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
    if(updateSource) // reset everything if it's a new source
    {
        this->source = source;
        currentPos = 0;
        a_elapsedTime = sf::Time::Zero;
    }
    if(!font) return; // a font must be loaded, to avoid undefined behaviors

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
                sf::String substr = source.toWideString().substr(start, length);
                sf::Color c = getColor(substr, isValid);
                if(isValid) current->color = c;
                else
                {
                    current->style = Image;
                    current->text = substr.toAnsiString();
                    TextChunk::add(chunks, current, last);
                }
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

void GameText::build()
{
    float boundY = csize*nlratio;
    sf::Text* t = nullptr;
    sf::Vector2f partPos(0, 0);

    // still forced to clear here
    for(size_t i = 0; i < drawables.size(); ++i)
        if(drawables[i] != nullptr) delete drawables[i];
    drawables.clear();
    bounds.left = 0;
    bounds.top = 0;
    bounds.width = 0;
    bounds.height = 0;

    MDrawable* dra = nullptr;

    for(size_t i = 0; i < chunks.size(); ++i)
    {
        if(chunks[i]->text.getSize() != 0)
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
                            t->setCharacterSize(csize);

                            if(font) t->setFont(*font);

                            t->setPosition(partPos);
                        }
                        else delete dra;

                        if(t) partPos.x += t->getLocalBounds().width;
                        if(partPos.x >= bounds.width) bounds.width = partPos.x;
                        if(chunks[i]->endsInNewline)
                        {
                            partPos.y += boundY;
                            partPos.x = 0;
                            bounds.height += boundY;
                        }
                        else if(i == chunks.size()-1) bounds.height += boundY;

                        if(t)
                        {
                            drawables.push_back(dra);
                            parts.push_back((sf::Drawable*)t);
                            t = nullptr;
                        }
                    }
                    break;
                }
            }
        }
    }
}

void GameText::clear()
{
    for(size_t i = 0; i < chunks.size(); ++i)
        delete chunks[i];
    chunks.clear();
    for(MDrawable* &i: drawables)
        if(i) delete i;
    drawables.clear();
    parts.clear(); // only clear parts
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
