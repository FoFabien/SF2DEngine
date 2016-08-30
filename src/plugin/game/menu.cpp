#include "menu.hpp"

#define _USE_MENU_
#ifdef _USE_MENU_
#include "../../engine.hpp"
#include "../../mlib/mlib.hpp"

#include <map>
std::map<std::string, int8_t> menu_script_id;

void Menu::initMenuScript()
{
    menu_script_id[""] = 0;
    menu_script_id["none"] = 0;
    menu_script_id["color"] = 1;
    menu_script_id["animation"] = 2;
    menu_script_id["sound"] = 3;
    menu_script_id["music"] = 4;
    menu_script_id["custom"] = 127;
}

Menu::Menu()
{
    pause = false;
    initialized = false;
    ID = 0;
    cChoice = 0;
    data = nullptr;
}

Menu::~Menu()
{
    for(MenuVisual &i : cursors) delete i.draw;

    for(MenuVisual &i : visuals) delete i.draw;

    for(MenuActor &i : choices)
    {
        delete i.v->draw;
        delete i.v;
    }

    delete data;
}

void Menu::init(int32_t id)
{
    if(initialized) return;

    if(id < 0 || id >= (int32_t)menu_database.size()) return;
    ID = id;
    MenuData* mdata = nullptr;
    switch(id)
    {
        case 6: // resolution
        case 9: // key bind
        case 11: // new game
        case 12: // load game
        case 13: // language
            initSpecial(); mdata = data; break; // additional processing for those menu IDs
        default: mdata = &menu_database[id]; break; // get the database pointer
    }

    pause = mdata->pause;

    // cursor
    for(MenuDataVisual &i : mdata->cursors)
    {
        MenuVisual tmp;
        processVisual(tmp, i);
        cursors.push_back(tmp);
    }

    // visuals
    for(MenuDataVisual &i : mdata->visuals)
    {
        MenuVisual tmp;
        processVisual(tmp, i);
        visuals.push_back(tmp);
    }

    // choices
    for(size_t i = 0; i < mdata->choices.size(); ++i)
    {
        MenuActor tmp;
        tmp.v = new MenuVisual();
        processActor(tmp, mdata->choices[i]);
        if(i == 0) tmp.selected = true;
        runScript(tmp, i, tmp.data->default_state);
        choices.push_back(tmp);
    }

    if(choices.empty()) return;

    initialized = true;
    cChoice = 0;
}

bool Menu::isInitialized() const
{
    return initialized;
}

void Menu::action()
{
    if(!initialized) return;

    #ifdef _USE_INPUT_
    static sf::FloatRect frect;
    static sf::Vector2i mposition;
    static sf::Vector2f dposition;

    if(cChoice >= 0 && cChoice < (int32_t)choices.size())
    {
        if(engine.input.isPressedNow(KUP)) setCurrentChoice(choices[cChoice].data->up);
        else if(engine.input.isPressedNow(KRIGHT)) setCurrentChoice(choices[cChoice].data->right);
        else if(engine.input.isPressedNow(KDOWN)) setCurrentChoice(choices[cChoice].data->down);
        else if(engine.input.isPressedNow(KLEFT)) setCurrentChoice(choices[cChoice].data->left);

        if(engine.input.isPressedNow(choices[cChoice].data->unselec_key))
        {
            cancelChoice(choices[cChoice].data->unselec_sound);
            return;
        }
        if(engine.input.isPressedNow(choices[cChoice].data->selec_key))
        {
            validChoice(choices[cChoice].data->selec_sound);
            return;
        }
    }
    if(engine.input.isMouseEnabled() && engine.input.isPressedNow(KLEFTCLICK))
    {
        sf::Vector2i mposition = engine.input.getMousePosition();
        if(mposition.x > -1 && mposition.y > -1)
        {
            for(size_t i = 0; i < choices.size(); ++i)
            {
                frect = choices[i].v->draw->getLocalBounds();
                dposition = choices[i].v->draw->getPosition();
                frect.left += dposition.x;
                frect.top  += dposition.y;
                if(mposition.x >= frect.left && mposition.y >= frect.top && mposition.x < frect.left + frect.width && mposition.y < frect.top + frect.height)
                {
                    if((int32_t)i == cChoice) validChoice(choices[cChoice].data->selec_sound);
                    else setCurrentChoice(i);
                    return;
                }
            }
        }
    }
    #endif
}

void Menu::update()
{
    if(!initialized) return;

    for(MenuVisual &i : cursors)
    {
        if(i.draw) i.draw->update();
    }

    for(MenuVisual &i : visuals)
    {
        if(i.draw) i.draw->update();
    }

    for(MenuActor &i : choices)
    {
        if(i.v && i.v->draw) i.v->draw->update();
    }
}

void Menu::draw()
{
    if(!initialized) return;

    sf::IntRect view = engine.getHUDBounds();

    for(MenuVisual &i : visuals) drawVisual(i, view);

    for(MenuActor &i : choices) drawVisual(*(i.v), view);

    int32_t tmpX;
    int32_t tmpY;
    for(MenuVisual &i : cursors)
    {
        if(cChoice >= 0 && cChoice < (int32_t)choices.size())
        {
            tmpX = i.data->offsetX;
            tmpY = i.data->offsetY;
            i.data->fixed = choices[cChoice].v->data->fixed; // segfault
            i.data->offsetX += choices[cChoice].v->data->offsetX;
            i.data->offsetY += choices[cChoice].v->data->offsetY;
            drawVisual(i, view);
            i.data->offsetX = tmpX;
            i.data->offsetY = tmpY;
        }
    }
}

bool Menu::isPausingGame() const
{
    return pause;
}

void Menu::setCurrentChoice(int32_t n)
{
    if(initialized && n >= 0 && n < (int32_t)choices.size())
    {
        if(cChoice >= 0 && cChoice < (int32_t)choices.size())
        {
            choices[cChoice].selected = false;
            runScript(choices[cChoice], cChoice, choices[cChoice].data->unselec_state);
        }
        cChoice = n;
        choices[cChoice].selected = true;
        runScript(choices[cChoice], cChoice, choices[cChoice].data->selec_state);
        #ifdef _USE_SOUND_
        engine.sounds.play(choices[cChoice].data->choice_sound);
        #endif
    }
}

void Menu::processVisual(MenuVisual& ref, MenuDataVisual& visual)
{
    ref.draw = new MDrawable();
    switch(visual.type)
    {
        case 10: break;
        case 11: case 16: // rich text, standard text
            #ifdef _USE_LANG_
            if(ref.draw->loadFromData(static_cast<MDrawable_type>(visual.type-SPECIAL_TYPE_OFFSET), engine.langs.getString(visual.s)))
            #else
            if(ref.draw->loadFromData(static_cast<MDrawable_type>(visual.type-SPECIAL_TYPE_OFFSET), visual.s))
            #endif
            {
                ref.draw->setCharacterSize(visual.size1);
                #ifdef _USE_FONT_
                ref.draw->setFont(*engine.fonts.useCurrentFont());
                #endif
            }
            break;
        case 12: // sprite
            ref.draw->loadFromData(static_cast<MDrawable_type>(visual.type-SPECIAL_TYPE_OFFSET), visual.s);
            break;
        case 13: // animation
            if(ref.draw->loadFromData(static_cast<MDrawable_type>(visual.type-SPECIAL_TYPE_OFFSET), visual.s))
            {
                ref.draw->play();
            }
            break;
        case 14: case 15: // rectangle & circle
            if(ref.draw->loadFromData(static_cast<MDrawable_type>(visual.type-SPECIAL_TYPE_OFFSET), ""))
            {
                ref.draw->setSize(visual.size1, visual.size2);
            }
            break;
        case 17: // game text
            #ifdef _USE_LANG_
            if(ref.draw->loadFromData(static_cast<MDrawable_type>(visual.type-SPECIAL_TYPE_OFFSET), engine.langs.getString(visual.s)))
            #else
            if(ref.draw->loadFromData(static_cast<MDrawable_type>(visual.type-SPECIAL_TYPE_OFFSET), visual.s))
            #endif
            {
                ref.draw->setCharacterSize(visual.size1);
                #ifdef _USE_FONT_
                ref.draw->setFont(*engine.fonts.useCurrentFont());
                #endif
                ref.draw->setFrameSpeed(sf::milliseconds(visual.size2));
                ref.draw->setSoundFile1(visual.s1);
                ref.draw->setSoundFile2(visual.s2);
                ref.draw->play();
            }
            break;
        default:
            if(ref.draw->loadFromMemory(visual.s))
            {
                switch(ref.draw->getType())
                {
                    case UNDEF_DRAW: break;
                    case TEXT: case STDTEXT:
                        #ifdef _USE_FONT_
                        ref.draw->setFont(*engine.fonts.useCurrentFont());
                        #endif
                        ref.draw->setCharacterSize(visual.size1);
                        break;
                    case SPRITE: break;
                    case ANIMATION:
                        ref.draw->play();
                        break;
                    case RECTANGLE: case CIRCLE:
                        ref.draw->setSize(visual.size1, visual.size2);
                        break;
                     case GAMETEXT:
                        #ifdef _USE_FONT_
                        ref.draw->setFont(*engine.fonts.useCurrentFont());
                        #endif
                        ref.draw->setCharacterSize(visual.size1);
                }
            }
        break;
    }
    ref.draw->setColor(visual.color);
    ref.data = &visual;
}

void Menu::processActor(MenuActor &ref, MenuDataActor& actor)
{
    ref.v = new MenuVisual();
    processVisual(*(ref.v), actor.v);
    ref.data = &actor;
    ref.selected = false;
}

void Menu::drawVisual(MenuVisual &ref, const sf::IntRect& view)
{
    if(ref.draw)
    {
        switch(ref.data->fixed)
        {
            default: ref.data->fixed = 0;
            case 0: ref.draw->setPosition(ref.data->offsetX, ref.data->offsetY); break;
            case 1: ref.draw->setPosition(ref.data->offsetX+view.left, ref.data->offsetY+view.top); break; // top left
            case 2: ref.draw->setPosition(ref.data->offsetX+(view.width/2)+view.left, ref.data->offsetY+view.top); break; // top center
            case 3: ref.draw->setPosition(ref.data->offsetX+view.width+view.left, ref.data->offsetY+view.top); break; // top right
            case 4: ref.draw->setPosition(ref.data->offsetX+view.left, ref.data->offsetY+(view.height/2)+view.top); break;
            case 5: ref.draw->setPosition(ref.data->offsetX+(view.width/2)+view.left, ref.data->offsetY+(view.height/2)+view.top); break;
            case 6: ref.draw->setPosition(ref.data->offsetX+view.width+view.left, ref.data->offsetY+(view.height/2)+view.top); break;
            case 7: ref.draw->setPosition(ref.data->offsetX+view.left, ref.data->offsetY+view.height+view.top); break;
            case 8: ref.draw->setPosition(ref.data->offsetX+(view.width/2)+view.left, ref.data->offsetY+view.height+view.top); break;
            case 9: ref.draw->setPosition(ref.data->offsetX+view.width+view.left, ref.data->offsetY+view.height+view.top); break;
        }
        ref.draw->draw();
    }
}

void Menu::runScript(MenuActor& ref, const int32_t &id, const std::string& script)
{
    std::vector<std::string> elems;
    mlib::split(script, {';'}, elems);

    for(std::string& i : elems)
    {
        std::vector<std::string> line;
        mlib::split(i, {'='}, line);

        if(!line.empty())
        {
            switch(menu_script_id[line[0]])
            {
                default: break; // do nothing
                case 1: // color
                    if(line.size() >= 2)
                    {
                        std::vector<std::string> args;
                        mlib::split(line[1], {','}, args);
                        if(args.size() >= 4) ref.v->draw->setColor(sf::Color(mlib::str2int(args[0]), mlib::str2int(args[1]), mlib::str2int(args[2]), mlib::str2int(args[3])));
                        else if(args.size() == 3) ref.v->draw->setColor(sf::Color(mlib::str2int(args[0]), mlib::str2int(args[1]), mlib::str2int(args[2])));
                    }
                    break;
                case 2: // animation
                    if(line.size() >= 2)
                    {
                        switch(mlib::str2int(line[1]))
                        {
                            default: ref.v->draw->pause(); break;
                            case 1: ref.v->draw->play(); break;
                            case 2: ref.v->draw->stop(); break;
                        }
                    }
                    break;
                case 3: // sound
                    if(line.size() >= 2)
                    {
                        #ifdef _USE_SOUND_
                        engine.sounds.play(line[1]);
                        #endif
                    }
                    break;
                case 4: // music
                    if(line.size() >= 2)
                    {
                        #ifdef _USE_MUSIC_
                        engine.musics.play(line[1]);
                        #endif
                    }
                    break;
                case 127: // custom
                    applyCustomRules(ref, id);
                    break;
            }
        }
    }
}

#endif
