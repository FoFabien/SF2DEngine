#include "mapeditor.hpp"
#include "../../version.h"
#include <SFML/Window/Event.hpp>
#include "../../engine/mlib/mlib.hpp"
#include <queue>
#include "../../shared/data.hpp"

#define MAP_VERSION 0x3530764d
#define HUD_TOOLBAR_H 42
#define ENGINE_SFML_VERSION "SFML v2.4.2"

#define TILE_COUNT 18

MapEditor engine;

#ifdef WINDOWS_VERSION
#include <windows.h>
#define DIAG_BUFFSIZE 1024
std::wstring OpenFileName(const std::wstring & prompt)
{
    wchar_t buffer[DIAG_BUFFSIZE] = {0};
    OPENFILENAME ofns = {0};
    ofns.lStructSize = sizeof( ofns );
    ofns.lpstrFile = buffer;
    ofns.nMaxFile = DIAG_BUFFSIZE;
    ofns.lpstrTitle = prompt.c_str();
    ofns.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    GetOpenFileName(&ofns);
    return buffer;
}

std::wstring SaveFileName(const std::wstring & prompt)
{
    wchar_t buffer[DIAG_BUFFSIZE] = {0};
    OPENFILENAME ofns = {0};
    ofns.lStructSize = sizeof( ofns );
    ofns.lpstrFile = buffer;
    ofns.nMaxFile = DIAG_BUFFSIZE;
    ofns.lpstrTitle = prompt.c_str();
    ofns.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    GetOpenFileName(&ofns);
    return buffer;
}
#endif

int main()
{
    engine.init();

    while(engine.isRunning())
    {
        engine.update();
        engine.render();
    }
    engine.term();

    return 0;
}

//#############################################################################

MapEditor::MapEditor()
{
    wDir = mlib::getWorkingDirectory();
    state = INIT;
    Out.resetTimestamp();
    Out.useTimestamp(true);
    Out.setFile("Out.txt", true);
    Out.setOutput(true, true);
    Out = "[DEBUG ENABLED]\n";
    Out = std::string("EG Engine Light v") + AutoVersion::STATUS + " " + AutoVersion::FULLVERSION_STRING + "\n";
    Out = std::string("Map Editor Build - ") + ENGINE_SFML_VERSION + "\n";

    // graphic
    sizeLimit = sf::Texture::getMaximumSize();
    wintitle = "Map editor";
    view[0].setCenter(400, 300);
    create(sf::VideoMode(800, 600), wintitle);
    win.setActive(false);
    rTex = &tex[0];
    color[0] = sf::Color(255, 255, 255, 255);
    color[1] = sf::Color(255, 255, 255, 255);
    refreshVideoMode();
    currentVideoMode = - 1;
    winmode = 0;
    winfps = 30;
    focus = true;

    if(winfps < 30)
    {
        Out = "FPS limit too low. Setting to 30...\n";
        winfps = 30;
    }

    win.setFramerateLimit(winfps);
    elapsedTime.restart();

    game_loop_maxupdates = 30;
    game_loop_updaterate = (1000.f / (float)game_loop_maxupdates);

    // editor
    current_map = "";
    textInput = NULL;
    input_state = 0;
    camvec[0] = 0;
    camvec[1] = 0;
    show_hud = true;
    show = 0;
    current_tile = 0;
    pen_mode = 0;
    grid_entity = false;
    backspr.setFillColor(sf::Color::Transparent);
    backspr.setOutlineColor(sf::Color::Cyan);
    backspr.setOutlineThickness(2);
    l_mouse_pressed = false;
    clipboard_entity = false;
    map = NULL;

    // others
    sprite0.setPosition(0, 0);
    sprite1.setPosition(0, 0);

    sprite0.setTexture(tex[0].getTexture());
    sprite1.setTexture(tex[1].getTexture());
}

MapEditor::~MapEditor()
{
    if(state != STOPPED)
    {
        Out = "WARNING: Engine forcibly stopped. Attempting a normal exit...\n";
        term();
    }

    for(size_t i = 0; i < buttons.size(); i++) delete buttons[i];
    delete map;
    delete textInput;

    Out = "Engine stopped\n";
    Out = "Running time : " + std::to_string(Out.t() / 3600) + " hours "
                                + std::to_string(Out.t()  / 60) + " minutes "
                                + std::to_string(Out.t()  % 60) + " seconds\n";
}

void MapEditor::stop()
{
    state = STOPPING;
}

bool MapEditor::hasFocus()
{
    return focus;
}

void MapEditor::init()
{
    Out = "Engine starting\n";
    cmap_pos = sf::Vector2f(400, 300);
    ctile_pos = sf::Vector2f(400, 300);
    rect.setFillColor(sf::Color(100, 100, 100, 100));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/new.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/load.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/save.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/map.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/tileset.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/light.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/change_tile.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/size.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/layer.png"));
    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/entity.png"));

    buttons.push_back(new Button(sf::Vector2i(15, 15), "editor_hud/pen.png"));
    buttons.push_back(new Button(sf::Vector2i(15, 15), "editor_hud/bucket.png"));

    buttons.push_back(new Button(sf::Vector2i(15, 15), "editor_hud/grid.png"));

    buttons.push_back(new Button(sf::Vector2i(32, 32), "editor_hud/script.png"));
    font.loadFromFile("editor_hud/font.ttf");
    text.setFont(font);
    text.setCharacterSize(12);

    state = RUNNING;
    Out = "Engine ready\n";
}

void MapEditor::term()
{
    if(state == STOPPED) return;
    state = STOPPED;
}

bool MapEditor::isRunning()
{
    return (state == RUNNING);
}

void MapEditor::update()
{
    static int32_t anUpdates;
    static int32_t anUpdateTime;
    if(!win.isOpen())
    {
        state = STOPPING;
        return;
    }

    anUpdates = 0;
    anUpdateTime = anUpdateClock.getElapsedTime().asMilliseconds();
    while((anUpdateTime - anUpdateNext) >= game_loop_updaterate && anUpdates++ < game_loop_maxupdates)
    {
        while (win.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                    win.close();
                    break;
                case sf::Event::Resized:
                    resize();
                    break;
                case sf::Event::LostFocus:
                    focus = false;
                    l_mouse_pressed = false;
                    break;
                case sf::Event::GainedFocus:
                    focus = true;
                    break;
                case sf::Event::TextEntered:
                    if(input_state != 0)
                    {
                        if(event.text.unicode < 127 && event.text.unicode > 31) inputStr[input_state%10] += static_cast<char>(event.text.unicode);
                        else if(event.text.unicode == 8 && !inputStr[input_state%10].empty()) inputStr[input_state%10].erase(inputStr[input_state%10].size()-1);
                        else if(event.text.unicode == 13 && textInput)
                        {
                            textInput->forceOk();
                            confirmInput();
                        }
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    if(focus)
                    {
                        l_mouse_pressed = false;
                        if(event.mouseButton.button == sf::Mouse::Left)
                        {
                            if(input_state == 0)
                            {
                                size_t i = 0;
                                for(; i < buttons.size(); ++i)
                                    if(show_hud && buttons[i]->click(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))
                                        break;
                                if(i != buttons.size())
                                {
                                    //Out = "button " + mlib::dec2str(i) + "\n";
                                    switch(i)
                                    {
                                        case 0: m_new(); break;
                                        case 1: m_load(); break;
                                        case 2: m_save(); break;
                                        case 3: m_map(); break;
                                        case 4: m_tileset(); break;
                                        case 5: m_showall(); break;
                                        case 6:
                                            if(map)
                                            {
                                                input_state = 40;
                                                inputStr[0] = "";
                                                textInput = new TextDial("Input the tileset filename (empty to delete)", &inputStr[0]);
                                            }
                                            break;
                                        case 7:
                                            if(map)
                                            {
                                                input_state = 10;
                                                inputStr[0] = "";
                                                textInput = new TextDial("Input the map width", &inputStr[0]);
                                            }
                                            break;
                                        case 8:
                                            if(map)
                                            {
                                                input_state = 20;
                                                inputStr[0] = "";
                                                textInput = new TextDial("Input the number of layer", &inputStr[0]);
                                            }
                                            break;
                                        case 9:
                                            if(map)
                                            {
                                                input_state = 30;
                                                inputStr[0] = "";
                                                textInput = new TextDial("Input the new entity's ID", &inputStr[0]);
                                            }
                                            break;
                                        case 10: pen_mode = 0; break;
                                        case 11: pen_mode = 1; break;
                                        case 12: grid_entity = !grid_entity; break;
                                        case 13:
                                            if(map)
                                            {
                                                input_state = 50;
                                                inputStr[0] = "";
                                                textInput = new TextDial("Input the script ID", &inputStr[0]);
                                            }
                                            break;
                                        default: break;
                                    }
                                }
                                else if(show == 0 && map) l_mouse_pressed = true;
                                else if(show != 0 && map) map->clickTileset(getMousePosition());
                            }
                            else
                            {
                                confirmInput();
                                l_mouse_pressed = false;
                            }
                        }
                        else if(event.mouseButton.button == sf::Mouse::Right && input_state == 0)
                        {
                            if(show == 0 && clipboard_entity)
                            {
                                clipboard_entity = false;
                                entity.id = 0;
                            }
                            else if(show == 0 && map) map->copyEntity(getMousePosition());
                            else if(show != 0 && map) map->changeTileInfo(getMousePosition());
                            l_mouse_pressed = false;
                        }
                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if(focus)
                    {
                        if(event.mouseButton.button == sf::Mouse::Left)
                        {
                            l_mouse_pressed = false;
                        }
                    }
                    break;
                case sf::Event::MouseWheelScrolled:
                    if(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel && map)
                    {
                        if(event.mouseWheelScroll.delta > 0) map->layerUp();
                        else if(event.mouseWheelScroll.delta < 0) map->layerDown();
                    }
                    break;
                case sf::Event::KeyPressed:
                    if(input_state == 0)
                    {
                        switch(event.key.code)
                        {
                            case sf::Keyboard::Up:
                                if(camvec[0] < 0 && camvec[0] > -60) camvec[0]-=2;
                                else if(camvec[0] >= 0) camvec[0] = -20;
                                break;
                            case sf::Keyboard::Right:
                                if(camvec[1] > 0 && camvec[1] < 60) camvec[1]+=2;
                                else if(camvec[1] <= 0) camvec[1] = 20;
                                break;
                            case sf::Keyboard::Down:
                                if(camvec[0] > 0 && camvec[0] < 60) camvec[0]+=2;
                                else if(camvec[0] <= 0) camvec[0] = 20;
                                break;
                            case sf::Keyboard::Left:
                                if(camvec[1] < 0 && camvec[1] > -60) camvec[1]-=2;
                                else if(camvec[1] >= 0) camvec[1] = -20;
                                break;
                            case sf::Keyboard::Tab:
                                show_hud = !show_hud;
                                break;
                            case sf::Keyboard::Z: case sf::Keyboard::W:
                                changeTile(0);
                                break;
                            case sf::Keyboard::Q: case sf::Keyboard::A:
                                changeTile(3);
                                break;
                            case sf::Keyboard::S:
                                changeTile(2);
                                break;
                            case sf::Keyboard::D:
                                changeTile(1);
                                break;
                            case sf::Keyboard::F1:
                                m_new();
                                break;
                            case sf::Keyboard::F2:
                                m_load();
                                break;
                            case sf::Keyboard::F3:
                                m_save();
                                break;
                            case sf::Keyboard::M: case sf::Keyboard::F4:
                                m_map();
                                break;
                            case sf::Keyboard::T: case sf::Keyboard::F5:
                                m_tileset();
                                break;
                            case sf::Keyboard::F6:
                                m_showall();
                                break;
                            case sf::Keyboard::F7:
                                if(map)
                                {
                                    input_state = 40;
                                    inputStr[0] = "";
                                    textInput = new TextDial("Input the tileset filename (empty to delete)", &inputStr[0]);
                                }
                                break;
                            case sf::Keyboard::F8:
                                if(map)
                                {
                                    input_state = 10;
                                    inputStr[0] = "";
                                    textInput = new TextDial("Input the map width", &inputStr[0]);
                                }
                                break;
                            case sf::Keyboard::F9:
                                if(map)
                                {
                                    input_state = 20;
                                    inputStr[0] = "";
                                    textInput = new TextDial("Input the number of layer", &inputStr[0]);
                                }
                                break;
                            case sf::Keyboard::F10:
                                if(map)
                                {
                                    input_state = 30;
                                    inputStr[0] = "";
                                    textInput = new TextDial("Input the new entity's ID", &inputStr[0]);
                                }
                                break;
                            default: break;
                        }
                    }
                    break;
                case sf::Event::KeyReleased:
                    if(input_state == 0)
                    {
                        switch(event.key.code)
                        {
                            case sf::Keyboard::Up:
                                if(camvec[0] < 0) camvec[0] = 0;
                                break;
                            case sf::Keyboard::Right:
                                if(camvec[1] > 0) camvec[1] = 0;
                                break;
                            case sf::Keyboard::Down:
                                if(camvec[0] > 0) camvec[0] = 0;
                                break;
                            case sf::Keyboard::Left:
                                if(camvec[1] < 0) camvec[1] = 0;
                                break;
                            default: break;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        moveGameView(camvec[1], camvec[0]);
        if(show == 0 && focus && l_mouse_pressed)
        {
            map->click(getMousePosition());
        }
        // Compute the next appropriate UpdateFixed time
        anUpdateNext += game_loop_updaterate;
    }
}

void MapEditor::render()
{
    if(state == STOPPING) return;

    tex[0].clear(sf::Color::Black);
    tex[1].clear(sf::Color::Transparent);
    rTex = &tex[0];
    tex[0].setView(view[0]);
    // view A
    if(map)
    {
        if(show == 0) map->drawLow();
        else map->drawTileset();
    }
    rTex = &tex[1];
    tex[1].setView(view[1]);
    // view B
    hud();
    drawCurrentTile();
    if(map)
    {
        if(show == 0) map->drawHigh();
    }

    elapsedTime.restart();
    tex[0].display();
    tex[1].display();

    sprite0.setColor(color[0]);
    sprite1.setColor(color[1]);
    win.clear();
    win.setView(view[1]);
    win.draw(sprite0);
    win.draw(sprite1);
    win.display();
}

void MapEditor::draw(sf::Drawable *ptr)
{
    if(!ptr) return;
    rTex->draw(*ptr);
}

void MapEditor::confirmInput()
{
    if(textInput)
    {
        if(textInput->click(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))
        {
            if(textInput->gotInput())
            {
                switch(input_state)
                {
                    case 10:
                        input_state = 11;
                        inputStr[1] = "";
                        delete textInput;
                        textInput = new TextDial("Input the map height", &inputStr[1]);
                        break;
                    case 11:
                        delete textInput;
                        textInput = NULL;
                        input_state = 0;
                        if(mlib::isNumber(inputStr[0]) && mlib::isNumber(inputStr[1]))
                        {
                            m_size(mlib::str2int(inputStr[0]), mlib::str2int(inputStr[1]));
                        }
                        break;
                    case 20:
                        delete textInput;
                        textInput = NULL;
                        input_state = 0;
                        if(mlib::isNumber(inputStr[0]))
                        {
                            m_layer(mlib::str2int(inputStr[0]));
                        }
                        break;
                    case 30:
                        delete textInput;
                        textInput = NULL;
                        input_state = 0;
                        if(mlib::isNumber(inputStr[0]))
                        {
                            m_entity(mlib::str2int(inputStr[0]));
                        }
                        break;
                    case 40:
                        if(inputStr[0].empty())
                        {
                            delete textInput;
                            textInput = NULL;
                            input_state = 0;
                            m_settile(inputStr[0], "0", "0", "0", "0");
                        }
                        else
                        {
                            input_state = 41;
                            inputStr[1] = "";
                            delete textInput;
                            textInput = new TextDial("Input the tile size width", &inputStr[1]);
                        }
                        break;
                    case 41:
                        input_state = 42;
                        inputStr[2] = "";
                        delete textInput;
                        textInput = new TextDial("Input the tile size height", &inputStr[2]);
                        break;
                    case 42:
                        input_state = 43;
                        inputStr[3] = "";
                        delete textInput;
                        textInput = new TextDial("Input the tileset width (in tile)", &inputStr[3]);
                        break;
                    case 43:
                        input_state = 44;
                        inputStr[4] = "";
                        delete textInput;
                        textInput = new TextDial("Input the tileset height (in tile)", &inputStr[4]);
                        break;
                    case 44:
                        delete textInput;
                        textInput = NULL;
                        input_state = 0;
                        m_settile(inputStr[0], inputStr[1], inputStr[2], inputStr[3], inputStr[4]);
                        Out = "OK\n";
                        break;
                    case 50:
                        delete textInput;
                        textInput = NULL;
                        input_state = 0;
                        if(mlib::isNumber(inputStr[0]))
                        {
                            m_script(mlib::str2int(inputStr[0]));
                        }
                        break;
                    default:
                        delete textInput;
                        textInput = NULL;
                        input_state = 0;
                        break;
                }
            }
            else // cancel
            {
                delete textInput;
                textInput = NULL;
                input_state = 0;
            }
        }
    }
    else
    {
        input_state = 0;
    }
}

sf::Vector2f MapEditor::getGameViewCenter()
{
    return view[0].getCenter();
}

void MapEditor::moveGameView(int32_t x, int32_t y)
{
    view[0].move(x, y);
}

void MapEditor::setGameViewCenter(int32_t x, int32_t y)
{
    view[0].setCenter(x, y);
}

sf::IntRect MapEditor::getViewRect()
{
    sf::IntRect r;
    r.left = view[0].getCenter().x - (view[0].getSize().x / 2);
    r.top = view[0].getCenter().y - (view[0].getSize().y / 2);
    r.width = view[0].getCenter().x + (view[0].getSize().x / 2);
    r.height = view[0].getCenter().y + (view[0].getSize().y / 2);
    return r;
}

void MapEditor::refreshVideoMode()
{
    modes = sf::VideoMode::getFullscreenModes();
    for(size_t i = 0; i < modes.size(); ++i)
    {
        if(modes[i].bitsPerPixel != 32)
        {
            modes.erase(modes.begin()+i);
            --i;
        }
    }
    #ifdef DEBUG_BUILD
    for(size_t i = 0; i < modes.size(); ++i)
    {
        Out = "Mode (" + mlib::dec2str(i) + "): " + mlib::dec2str(modes[i].width) + "x" + mlib::dec2str(modes[i].height) + "\n";
    }
    #endif
}

void MapEditor::resize()
{
    bool limited = false;
    sf::Vector2f s(win.getSize().x, win.getSize().y);
    if(winmode == 0)
    {
        if(s.x > sizeLimit)
        {
            s.x = sizeLimit;
            limited = true;
        }
        if(s.y > sizeLimit)
        {
            s.y = sizeLimit;
            limited = true;
        }
    }
    if(limited) win.setSize(sf::Vector2u(s.x, s.y));

    view[0].setSize(s);
    view[1].setSize(s);

    view[1].setCenter(s.x/2, s.y/2);

    tex[0].create(s.x, s.y);
    tex[1].create(s.x, s.y);

    sprite0.setTextureRect(sf::IntRect(0, 0, s.x, s.y));
    sprite1.setTextureRect(sf::IntRect(0, 0, s.x, s.y));
}

void MapEditor::create(sf::VideoMode mode, const sf::String &title, sf::Uint32 style, const sf::ContextSettings &settings)
{
    sizeLimit = sf::Texture::getMaximumSize();
    if(mode.width > sizeLimit) mode.width = sizeLimit;
    if(mode.height > sizeLimit) mode.height = sizeLimit;
    win.create(mode, title, style, settings);
    win.setFramerateLimit(winfps);
    wintitle = title;
    tex[0].create(mode.width, mode.height);
    tex[1].create(mode.width, mode.height);
    resize();
}

std::string MapEditor::getPath(std::string name)
{
    if(trans_tab.find(name) == trans_tab.end())
    {
        std::string tmp = mlib::wstr2str(OpenFileName(L"Please select " + mlib::str2wstr(name)));
        if(tmp.empty()) return "";
        trans_tab[name] = tmp;
        Out = "Path added: " + tmp + "\n";
    }
    return trans_tab[name];
}

void MapEditor::removePath(std::string name)
{
    std::map<std::string, std::string>::const_iterator it = trans_tab.find(name);
    if(it != trans_tab.end()) trans_tab.erase(it);
}

bool MapEditor::saveTable(std::string file)
{
    std::ofstream f(file.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f)
    {
        MessageBox(NULL, (LPCWSTR)L"Failed to save the list of file associated with the map", (LPCWSTR)L"Message", MB_OK);
        return false;
    }
    char cstop = 0;
    for(std::map<std::string, std::string>::const_iterator it = trans_tab.begin(); it != trans_tab.end(); ++it)
    {
        if(it != trans_tab.begin()) f.write("\n", 1);
        for(size_t i = 0; i < it->first.size(); ++i)
            f.write(&(it->first)[i], 1);
        f.write(&cstop, 1);
        for(size_t i = 0; i < it->second.size(); ++i)
            f.write(&(it->second)[i], 1);
    }
    return true;
}

bool MapEditor::loadTable(std::string file)
{
    std::ifstream f(file.c_str(), std::ios::in | std::ios::binary);
    if(!f)
    {
        MessageBox(NULL, (LPCWSTR)L"No file list found", (LPCWSTR)L"Message", MB_OK);
        return false;
    }
    bool toggle = false;
    std::string str1, str2;
    char c;

    trans_tab.clear();
    while(f.good())
    {
        if(!toggle)
        {
            str1.clear();
            do
            {
                f.read(&c, 1);
                if(c != 0 && f.good()) str1 += c;
            }while(c != 0 && f.good());
            toggle = true;
        }
        else
        {
            str2.clear();
            do
            {
                f.read(&c, 1);
                if(c != '\n' && f.good()) str2 += c;
            }while(c != '\n' && f.good());
            trans_tab[str1] = str2;
            Out = str1 + "=" + str2 + "\n";
            toggle = false;
        }
    }
    return true;
}

void MapEditor::hud()
{
    if(show_hud)
    {
        rect.setPosition(0, win.getSize().y - HUD_TOOLBAR_H);
        rect.setSize(sf::Vector2f(win.getSize().x, HUD_TOOLBAR_H));
        draw(&rect);
        for(size_t i = 0; i < buttons.size(); ++i)
        {
            if(i < 10) buttons[i]->setPosition(5 + i*37, 5 + win.getSize().y - HUD_TOOLBAR_H);
            else if(i >= 10 && i < 12) buttons[i]->setPosition(412 + (i-10)*14, 5 + win.getSize().y - HUD_TOOLBAR_H);
            else if(i == 12) buttons[i]->setPosition(412, 19 + win.getSize().y - HUD_TOOLBAR_H);
            else if(i == 13) buttons[i]->setPosition(375, 5 + win.getSize().y - HUD_TOOLBAR_H);
            buttons[i]->draw();
        }

        // block
        {
            std::string str;
            if(engine.pen_mode == 0) str += "pen\n";
            else str += "fill\n";
            if(engine.grid_entity) str += "tile";
            else str += "free";
            text.setString(str);
            text.setPosition(450, win.getSize().y - HUD_TOOLBAR_H + 1);
            draw(&text);
        }

        if(focus && show == 0)
        {
            sf::Vector2i pos = getMousePosition();
            if(pos.x != -1)
            {
                sf::IntRect cam = getViewRect();
                pos.x += cam.left;
                pos.y += cam.top;
                if(map && map->hasTileSet())
                text.setString("x=" + std::to_string(pos.x) + " (" + std::to_string(pos.x/map->getTileSize().x) +
                               ")\ny=" + std::to_string(pos.y) + " (" + std::to_string(pos.y/map->getTileSize().y) + ")");
                else text.setString("x=" + std::to_string(pos.x) + "\ny=" + std::to_string(pos.y));
            }
            else text.setString("x=?\ny=?");
        }
        else text.setString("x=?\ny=?");
        text.setPosition(490, win.getSize().y - HUD_TOOLBAR_H);
        draw(&text);

        if(map)
        {
            text.setString("Map size: " + std::to_string(map->getSize().x) + "x" + std::to_string(map->getSize().y)
                           + "\nlayer: " + std::to_string(map->getLayerCount()) + " curent: " + std::to_string(map->getSelectedLayer())
                           + " | script: " + std::to_string(map->script_id)
                           );
            text.setPosition(600, win.getSize().y - HUD_TOOLBAR_H);
            draw(&text);
        }
        else
        {
            text.setString("No map loaded");
            text.setPosition(win.getSize().x - 150, win.getSize().y - HUD_TOOLBAR_H);
            draw(&text);
        }
        text.setString("Tab: Hide HUD\nRight click: Delete selected/Copy Entity");
        text.setPosition(2, 2);
        draw(&text);
    }
    else
    {
        text.setString("Tab: Show HUD");
        text.setPosition(2, 2);
        draw(&text);
    }
    if(textInput)
    {
        textInput->setPosition((win.getSize().x/2)-100, (win.getSize().y/2)-50);
        textInput->draw();
    }
}

sf::Vector2i MapEditor::getMousePosition()
{
    sf::Vector2i pos = sf::Mouse::getPosition(win);
    if(pos.x < 0 || pos.x >= (int32_t)win.getSize().x || pos.y < 0|| pos.y >= (int32_t)win.getSize().y) return sf::Vector2i(-1, -1);
    return pos;
}

void MapEditor::drawCurrentTile()
{
    if(focus && show == 0 && map && map->hasTileSet() && !clipboard_entity)
    {
        sf::Vector2i pos = getMousePosition();
        if(pos.x != -1)
        {
            const sf::Texture* tileset = map->getTexture();

            backspr.setPosition(pos.x, pos.y);
            backspr.setSize(sf::Vector2f(map->getTileSize().x, map->getTileSize().y));
            draw(&backspr);

            if(current_tile != 0)
            {
                tilespr.setTexture(*tileset);
                tilespr.setTextureRect(sf::IntRect((current_tile % map->getTilesetWidth()) * map->getTileSize().x,
                                       (current_tile / map->getTilesetWidth()) * map->getTileSize().y,
                                       map->getTileSize().x, map->getTileSize().y));
                tilespr.setPosition(pos.x, pos.y);
                draw(&tilespr);
            }
        }
    }
}

void MapEditor::changeTile(char dir)
{
    if(map && map->hasTileSet())
    {
        int32_t max = map->getTilesetHeight() * map->getTilesetWidth();
        switch(dir)
        {
            case 0: current_tile-=map->getTilesetWidth(); if(current_tile < 0) current_tile = max+current_tile; break;
            case 1: ++current_tile; if(current_tile >= max) current_tile = 0; break;
            case 2: current_tile+=map->getTilesetWidth(); if(current_tile >= max) current_tile -= max; break;
            case 3: --current_tile; if(current_tile < 0) current_tile = max-1; break;
            default: break;
        }
    }
}

void MapEditor::m_new()
{
    if(map)
    {
        if(MessageBox(NULL,  (LPCWSTR)L"A map is already loaded. Continue ?", (LPCWSTR)L"Message", MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
            return;
        delete map;
        current_map = "";
    }
    map = new Map();
    map->initEmpty();
    trans_tab.clear();
    setGameViewCenter(400, 300);
    cmap_pos = sf::Vector2f(400, 300);
    ctile_pos = sf::Vector2f(400, 300);
    camvec[0] = 0;
    camvec[1] = 0;
    show = 0;
    current_tile = 0;
    clipboard_entity = false;
}

void MapEditor::m_load()
{
    if(map)
    {
        if(MessageBox(NULL, (LPCWSTR)L"A map is already loaded. Continue ?", (LPCWSTR)L"Message", MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
            return;
    }
    std::string tmp = mlib::wstr2str(OpenFileName(L"Load a map"));
    mlib::setWorkingDirectory(wDir);
    if(tmp.empty()) return;

    delete map;
    map = new Map();
    map->clear();
    trans_tab.clear();
    loadTable(tmp + "_filelist");
    if(!map->load(tmp))
    {
        MessageBox(NULL, (LPCWSTR)L"Load failed", (LPCWSTR)L"Message", MB_OK);
        delete map;
        map = NULL;
        current_map = "";
    }
    else current_map = tmp;
    setGameViewCenter(400, 300);
    cmap_pos = sf::Vector2f(400, 300);
    ctile_pos = sf::Vector2f(400, 300);
    camvec[0] = 0;
    camvec[1] = 0;
    show = 0;
    current_tile = 0;
    clipboard_entity = false;
}

void MapEditor::m_save()
{
    if(!map)
    {
        MessageBox(NULL,  (LPCWSTR)L"No map loaded.", (LPCWSTR)L"Message", MB_OK);
        return;
    }
    if(!current_map.empty() && MessageBox(NULL,  (LPCWSTR)L"Save with the same name ?", (LPCWSTR)L"Message", MB_YESNO | MB_ICONQUESTION) == IDYES)
    {
        saveTable(current_map + "_filelist");
        if(!map->save(current_map)) MessageBox(NULL, (LPCWSTR)L"Save failed", (LPCWSTR)L"Message", MB_OK);
        else MessageBox(NULL, (LPCWSTR)L"Success", (LPCWSTR)L"Message", MB_OK);
    }
    else
    {
        std::string tmp = mlib::wstr2str(SaveFileName(L"Save the map"));
        if(tmp.empty()) return;
        saveTable(tmp + "_filelist");
        if(!map->save(tmp)) MessageBox(NULL, (LPCWSTR)L"Save failed", (LPCWSTR)L"Message", MB_OK);
        else MessageBox(NULL, (LPCWSTR)L"Success", (LPCWSTR)L"Message", MB_OK);
    }
    mlib::setWorkingDirectory(wDir);
}

void MapEditor::m_map()
{
    if(!map) return;
    if(show != 0) ctile_pos = getGameViewCenter();

    setGameViewCenter(cmap_pos.x, cmap_pos.y);
    show = 0;
}

void MapEditor::m_tileset()
{
    if(!map) return;
    if(show == 0) cmap_pos = getGameViewCenter();

    setGameViewCenter(ctile_pos.x, ctile_pos.y);
    show = 1;
}

void MapEditor::m_showall()
{
    if(map) map->toggleShowAll();
}

void MapEditor::m_settile(const std::string &filename, std::string w, std::string h, std::string tw, std::string th)
{
    if(!map) return;
    if(filename.empty())
    {
        map->deleteTileset();
        map->updateTileInfo();
        return;
    }
    if(!mlib::isNumber(w) || !mlib::isNumber(h) || !mlib::isNumber(tw) || !mlib::isNumber(th)) return;
    if(mlib::str2int(w) <= 1 || mlib::str2int(h) <= 1) return;
    if(mlib::str2int(tw) <= 0 || mlib::str2int(th) <= 0) return;

    if(!map->loadTileset(filename)) MessageBox(NULL, (LPCWSTR)L"Failed to load the tileset", (LPCWSTR)L"Message", MB_OK);
    else
    {
        map->setTilesize(mlib::str2int(w), mlib::str2int(h));
        map->setTilesetSize(mlib::str2int(tw), mlib::str2int(th));
        map->updateTileInfo();
    }
}

void MapEditor::m_size(int32_t w, int32_t h)
{
    if(!map) return;

    if(w <= 0)
    {
        MessageBox(NULL, (LPCWSTR)L"Invalid width (<= 0)", (LPCWSTR)L"Message", MB_OK);
        return;
    }
    if(h <= 0)
    {
        MessageBox(NULL, (LPCWSTR)L"Invalid height (<= 0)", (LPCWSTR)L"Message", MB_OK);
        return;
    }
    map->setSize(w, h);
}

void MapEditor::m_layer(int32_t l)
{
    if(!map) return;
    if(l <= 0) return;
    map->setLayerCount(l);
}

void MapEditor::m_entity(int32_t id)
{
    if(!map || id < 0) return;
    if(clipboard_entity && MessageBox(NULL,  (LPCWSTR)L"An entity is currently selected. Continue ?", (LPCWSTR)L"Message", MB_YESNO | MB_ICONQUESTION) == IDNO)
        return;

    entity.id = id;
    clipboard_entity = true;
}

void MapEditor::m_script(int32_t id)
{
    if(!map) return;
    map->script_id = id;
}

//#############################################################################

Map::Map()
{
    hover_rect.setFillColor(sf::Color::Transparent);
    hover_rect.setOutlineColor(sf::Color::Yellow);
    hover_rect.setOutlineThickness(2);
    select_rect.setFillColor(sf::Color::Transparent);
    select_rect.setOutlineColor(sf::Color::Red);
    select_rect.setOutlineThickness(2);

    etext.setFont(engine.font);
    etext.setCharacterSize(10);
    epos.setPointCount(12);
    epos.setPoint(0 , sf::Vector2f(-1, -6));
    epos.setPoint(1 , sf::Vector2f(1, -6));
    epos.setPoint(2 , sf::Vector2f(1, -1));
    epos.setPoint(3 , sf::Vector2f(6, -1));
    epos.setPoint(4 , sf::Vector2f(6, 1));
    epos.setPoint(5 , sf::Vector2f(1, 1));
    epos.setPoint(6 , sf::Vector2f(1, 6));
    epos.setPoint(7 , sf::Vector2f(-1, 6));
    epos.setPoint(8 , sf::Vector2f(-1, 1));
    epos.setPoint(9 , sf::Vector2f(-6, 1));
    epos.setPoint(10, sf::Vector2f(-6, -1));
    epos.setPoint(11, sf::Vector2f(-1, -1));

    epos.setFillColor(sf::Color::Red);
    einfo.setFillColor(sf::Color(120, 120, 120, 120));
    tile_info.loadFromFile("editor_hud/tile_info.png");
    tile_info_spr.setTexture(tile_info);

    clear();
}

Map::~Map()
{
    clear();
}

bool Map::load(const std::string &path)
{
    std::ifstream f(path.c_str(), std::ios::in | std::ios::binary);
    if(!f)
    {
        Out = "Failed to open \"" + path + "\"\n";
        return false;
    }

    uint32_t tmp;
    char c;
    clear();
    f.read((char*)&tmp, 4);
    if(tmp != MAP_VERSION) return false;
    f.read((char*)&script_id, 4);
    f.read((char*)&size.x, 4);
    f.read((char*)&size.y, 4);
    f.read((char*)&tilesize.x, 4);
    f.read((char*)&tilesize.y, 4);
    f.read((char*)&layer_count, 1);

    if(tilesize.x <= 1 || tilesize.y <= 1) return false;

    tmp = 0;
    f.read((char*)&tmp, 1);
    if(tmp != 0)
    {
        do
        {
            if(!f.good()) return false;
            f.read(&c, 1);
            if(c != 0) tileset_file += c;
        }while(c != 0);

        f.read((char*)&tw, 1);
        if(tw == 0) return false;
        f.read((char*)&th, 1);
        if(th == 0) return false;

        if(!tileset.loadFromFile(engine.getPath(tileset_file)))
            return false;
        sprite.setTexture(tileset);
        hasTileset = true;

        for(uint32_t i = 0; i < tw*th; i++)
        {
            f.read(&c, 1);
            t_state.push_back(c);
        }

        for(uint32_t i = 0; i < layer_count; ++i)
        {
            tiles.push_back(std::vector<uint16_t>());
            uint16_t tile_id;
            for(uint32_t x = 0; x < size.x; ++x)
            {
                for(uint32_t y = 0; y < size.y; ++y)
                {
                    if(!f.good()) return false;
                    f.read((char*)&tile_id, 2);
                    tiles.back().push_back(tile_id);
                }
            }
        }
    }

    tmp = 0;
    f.read((char*)&tmp, 2);

    for(uint16_t i = 0; i < layer_count; ++i)
         entities.push_back(std::vector<Entity>());

    for(uint32_t i = 0; i < tmp; i++)
    {
        if(!f.good()) return false;

        Entity e;
        uint8_t l;

        f.read((char*)&e.id, 4);
        f.read((char*)&e.x, 4);
        f.read((char*)&e.y, 4);
        f.read((char*)&l, 1);

        if(l >= 0 && l <= (int32_t)layer_count)
            entities[l].push_back(e);
    }
    Out = "Map loaded from " + path + "\n";
    return true;
}

bool Map::save(const std::string &path)
{
    std::ofstream f(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if(!f)
    {
        Out = "Failed to open \"" + path + "\"\n";
        return false;
    }

    uint32_t tmp;
    char c;

    tmp = MAP_VERSION;
    f.write((char*)&tmp, 4);
    f.write((char*)&script_id, 4);
    f.write((char*)&size.x, 4);
    f.write((char*)&size.y, 4);
    f.write((char*)&tilesize.x, 4);
    f.write((char*)&tilesize.y, 4);
    f.write((char*)&layer_count, 1);

    c = hasTileset;
    f.write(&c, 1);
    if(hasTileset)
    {
        for(size_t i = 0; i < tileset_file.size(); ++i)
            f.write(&tileset_file[i], 1);
        tmp = 0;
        f.write((char*)&tmp, 1);
        f.write((char*)&tw, 1);
        f.write((char*)&th, 1);

        for(uint32_t i = 0; i < th*tw; i++)
            f.write((char*)&t_state[i], 1);

        for(uint32_t l = 0; l < layer_count; ++l)
        {
            for(uint32_t x = 0; x < size.x; ++x)
            {
                for(uint32_t y = 0; y < size.y; ++y)
                {
                    f.write((char*)&tiles[l][y+x*size.y], 2);
                }
            }
        }
    }

    tmp = 0;
    for(size_t i = 0; i < entities.size(); ++i)
        tmp += entities[i].size();
    f.write((char*)&tmp, 2);

    for(uint32_t l = 0; l < layer_count; ++l)
    {
        for(size_t i = 0; i < entities[l].size(); ++i)
        {
            f.write((char*)&entities[l][i].id, 4);
            f.write((char*)&entities[l][i].x, 4);
            f.write((char*)&entities[l][i].y, 4);
            f.write((char*)&l, 1);
        }
    }

    Out = "Map saved to " + path + "\n";
    return true;
}

void Map::drawLow()
{
    if(hasTileset)
    {
        sf::IntRect r = engine.getViewRect();

        bool hover = false;
        sf::Vector2i pos;
        if(engine.hasFocus() && !engine.clipboard_entity)
        {
            pos = engine.getMousePosition();
            if(pos.x != -1)
            {
                pos += sf::Vector2i(r.left, r.top);
                if(pos.x >= 0 && pos.y >= 0)
                {
                    pos.x /= tilesize.x;
                    pos.y /= tilesize.y;
                    if(pos.x < (int32_t)size.x && pos.y < (int32_t)size.y) hover = true;
                }
            }
        }

        r.left = (r.left / (int32_t)tilesize.x) - 1;
        r.top = (r.top / (int32_t)tilesize.y) - 1;
        r.width = (r.width / (int32_t)tilesize.x) + 1;
        r.height = (r.height / (int32_t)tilesize.y) + 1;

        if(r.left < 0) r.left = 0;
        if(r.top < 0) r.top = 0;
        if(r.width >= (int32_t)size.x) r.width = size.x - 1;
        if(r.height >= (int32_t)size.y) r.height = size.y - 1;

        for(uint8_t l = 0; l < layer_count && (show_all || (!show_all && l <= selected_layer + 1)); ++l)
        {
            if(show_all) sprite.setColor(sf::Color(255, 255, 255, 255));
            else if(l < selected_layer) sprite.setColor(sf::Color(150, 150, 150, 255));
            else if(l > selected_layer) sprite.setColor(sf::Color(255, 255, 255, 100));
            else sprite.setColor(sf::Color(255, 255, 255, 255));

            for(int32_t i = r.left; i <= r.width; ++i)
            {
                for(int32_t j = r.top; j <= r.height; ++j)
                {
                    if(tiles[l][j+i*size.y] != 0)
                    {
                        sprite.setTextureRect(sf::IntRect((tiles[l][j+i*size.y] % tw) * tilesize.x,
                                                        (tiles[l][j+i*size.y] / tw) * tilesize.y,
                                                        tilesize.x, tilesize.y));
                        sprite.setPosition(i*tilesize.x, j*tilesize.y);
                        engine.draw(&sprite);
                    }
                }
            }

            if(l == selected_layer)
            {
                if(hover)
                {
                    hover_rect.setSize(sf::Vector2f(tilesize.x, tilesize.y));
                    hover_rect.setPosition(pos.x*tilesize.x, pos.y*tilesize.y);
                    engine.draw(&hover_rect);
                }
                for(size_t e = 0; e < entities[l].size(); e++)
                {
                    epos.setPosition(entities[l][e].x, entities[l][e].y);
                    engine.draw(&epos);
                    std::string ename;
                    if(entities[l][e].id < 0 || entities[l][e].id >= ENTITY_COUNT) ename = "[" + std::to_string(entities[l][e].id) + "] ???";
                    else ename = "[" + std::to_string(entities[l][e].id) + "] " + entity_test_database[entities[l][e].id];
                    ename = ename + "\n" + std::to_string(entities[l][e].x) + "x" + std::to_string(entities[l][e].y);
                    etext.setString(ename);
                    einfo.setSize(sf::Vector2f(ename.size()*3, 32));
                    einfo.setPosition(entities[l][e].x, entities[l][e].y);
                    engine.draw(&einfo);
                    etext.setPosition(entities[l][e].x+2, entities[l][e].y+2);
                    engine.draw(&etext);
                }
            }
        }
    }
}

void Map::drawHigh()
{
    if(engine.clipboard_entity && engine.hasFocus())
    {
        sf::Vector2i pos = engine.getMousePosition();
        if(pos.x != -1)
        {
            epos.setPosition(pos.x, pos.y);
            engine.draw(&epos);
        }
    }
}

void Map::drawTileset()
{
    if(hasTileset && tw > 0 && tilesize.y > 0)
    {
        bool hover = false;
        sf::Vector2i pos;
        sf::IntRect camera = engine.getViewRect();
        if(engine.hasFocus())
        {
            pos = engine.getMousePosition();
            if(pos.x != -1)
            {
                pos += sf::Vector2i(camera.left, camera.top);
                if(pos.x >= 0 && pos.y >= 0)
                {
                    pos.x /= tilesize.x;
                    pos.y /= tilesize.y;
                    if(pos.x < tw && pos.y < th) hover = true;
                }
            }
        }
        sprite.setColor(sf::Color(255, 255, 255, 255));
        sprite.setTexture(tileset, true);
        sprite.setPosition(0, 0);
        engine.draw(&sprite);


        for(uint32_t i = 0; i < tw; ++i)
        {
            for(int32_t j = 0; j < th; ++j)
            {
                tile_info_spr.setPosition(((i+1)*tilesize.x)-8, ((j+1)*tilesize.y)-8);
                tile_info_spr.setTextureRect(sf::IntRect(t_state[i+j*tw]*8, 0, 8, 8));
                engine.draw(&tile_info_spr);
            }
        }

        if(hover)
        {
            hover_rect.setSize(sf::Vector2f(tilesize.x, tilesize.y));
            hover_rect.setPosition(pos.x*tilesize.x, pos.y*tilesize.y);
            engine.draw(&hover_rect);
        }
        select_rect.setSize(sf::Vector2f(tilesize.x, tilesize.y));
        select_rect.setPosition((engine.current_tile%tw)*tilesize.x, (engine.current_tile/tw)*tilesize.y);
        engine.draw(&select_rect);
    }
}

void Map::copyEntity(sf::Vector2i pos)
{
    if(pos.x < 0) return;
    sf::IntRect cam = engine.getViewRect();
    pos.x += cam.left;
    pos.y += cam.top;
    if(selected_layer < layer_count)
    {
        for(size_t i = 0; i < entities[selected_layer].size(); ++i)
        {
            sf::Vector2i np;
            np.x = entities[selected_layer][i].x - pos.x;
            np.y = entities[selected_layer][i].y - pos.y;
            np.x = np.x * np.x;
            np.y = np.y * np.y;
            if(np.x < 25 && np.y < 25)
            {
                engine.clipboard_entity = true;
                engine.entity = entities[selected_layer][i];
                return;
            }
        }
    }
}

void Map::clear()
{
    path = "";
    script_id = -1;
    size.x = 0;
    size.y = 0;
    tilesize.x = 0;
    tilesize.y = 0;
    layer_count = 0;
    selected_layer = 0;
    t_state.clear();
    hasTileset = false;
    tileset_file = "";
    tw = 0;
    th = 0;
    tiles.clear();
    entities.clear();
    show_all = true;
}

sf::Vector2u Map::getSize()
{
    return size;
}

uint8_t Map::getLayerCount()
{
    return layer_count;
}

uint8_t Map::getSelectedLayer()
{
    return selected_layer;
}

void Map::layerUp()
{
    if(layer_count <= 1) return;
    if(selected_layer == layer_count-1) selected_layer = 0;
    else ++selected_layer;
}

void Map::layerDown()
{
    if(layer_count <= 1) return;
    if(selected_layer == 0) selected_layer = layer_count-1;
    else --selected_layer;
}

void Map::initEmpty()
{
    clear();
    layer_count = 3;
    size.x = 10;
    size.y = 10;
    tiles.resize(3, std::vector<uint16_t>(100, 0));
    entities.resize(3, std::vector<Entity>());
}

void Map::toggleShowAll()
{
    show_all = !show_all;
}

void Map::setSize(int32_t x, int32_t y)
{
    if(x < 1 || y < 1 || (x == (int32_t)size.x && y == (int32_t)size.y)) return;
    std::vector<std::vector<uint16_t> > cpy;

    for(size_t l = 0; l < tiles.size(); ++l)
    {
        cpy.push_back(std::vector<uint16_t>(x*y, 0));
        for(uint32_t i = 0; i < size.x && i < (uint32_t)x; ++i)
        {
            for(uint32_t j = 0; j < size.x && j < (uint32_t)y; ++j)
            {
                cpy[l][j+i*y] = tiles[l][j+i*size.y];
            }
        }
    }
    size.x = x;
    size.y = y;
    tiles = cpy;
}

void Map::setLayerCount(uint32_t lc)
{
    if(lc < 0 || lc == layer_count) return;

    if(lc < tiles.size())
    {
        while(tiles.size() > lc) tiles.erase(tiles.begin()+tiles.size()-1);
        while(entities.size() > lc) entities.erase(entities.begin()+entities.size()-1);
    }
    else
    {
        while(tiles.size() < lc) tiles.push_back(std::vector<uint16_t>(size.x*size.y, 0));
        while(entities.size() < lc) entities.push_back(std::vector<Entity>());
    }

    layer_count = lc;
    if(selected_layer >= layer_count) selected_layer = layer_count - 1;
}

void Map::deleteTileset()
{
    if(hasTileset)
    {
        engine.removePath(tileset_file);
        sprite = sf::Sprite();
        tileset =sf::Texture();
        hasTileset = false;
    }
}

bool Map::loadTileset(const std::string &name)
{
    sf::Texture cpy;
    if(!cpy.loadFromFile(engine.getPath(name)))
    {
        engine.removePath(name);
        return false;
    }
    tileset = cpy;
    sprite.setTexture(tileset);
    hasTileset = true;
    tileset_file = name;
    return true;
}

void Map::setTilesize(int32_t w, int32_t h)
{
    if(w >= 1) tilesize.x = w;
    if(h >= 1) tilesize.y = h;
}

void Map::setTilesetSize(int32_t tile_w, int32_t tile_h)
{
    if(tile_w >= 1) tw = tile_w;
    if(tile_h >= 1) th = tile_h;
}

void Map::updateTileInfo()
{
    if(!hasTileset)
    {
        t_state.clear();
        return;
    }
    t_state.resize(tw*th, 0);
}

bool Map::clickTileset(sf::Vector2i pos)
{
    if(pos.x < 0 || !hasTileset) return false;
    sf::IntRect cam = engine.getViewRect();
    pos.x += cam.left;
    pos.y += cam.top;
    if(pos.x < 0 || pos.y < 0) return false;
    pos.x /= tilesize.x;
    pos.y /= tilesize.y;
    if(pos.x >= tw || pos.y >= th) return false;
    engine.current_tile = pos.x + pos.y * tw;
    return true;
}

bool Map::changeTileInfo(sf::Vector2i pos)
{
    if(pos.x < 0 || !hasTileset) return false;
    sf::IntRect cam = engine.getViewRect();
    pos.x += cam.left;
    pos.y += cam.top;
    if(pos.x < 0 || pos.y < 0) return false;
    pos.x /= tilesize.x;
    pos.y /= tilesize.y;
    if(pos.x >= tw || pos.y >= th) return false;
    t_state[pos.x + pos.y * tw] = (t_state[pos.x + pos.y * tw] + 1) % TILE_COUNT;
    return true;
}

bool Map::click(sf::Vector2i pos)
{
    if(pos.x < 0) return false;
    sf::IntRect cam = engine.getViewRect();
    pos.x += cam.left;
    pos.y += cam.top;

    if(engine.clipboard_entity && selected_layer < layer_count)
    {
        entities[selected_layer].push_back(engine.entity);
        entities[selected_layer].back().x = pos.x;
        entities[selected_layer].back().y = pos.y;

        if(engine.grid_entity && hasTileset)
        {
            entities[selected_layer].back().x -= (entities[selected_layer].back().x % tilesize.x);
            entities[selected_layer].back().y -= (entities[selected_layer].back().y % tilesize.y);
        }
        engine.clipboard_entity = false;
        engine.l_mouse_pressed = false;
        return true;
    }

    for(size_t i = 0; selected_layer < layer_count && i < entities[selected_layer].size(); ++i)
    {
        sf::Vector2i np;
        np.x = entities[selected_layer][i].x - pos.x;
        np.y = entities[selected_layer][i].y - pos.y;
        np.x = np.x * np.x;
        np.y = np.y * np.y;
        if(np.x < 25 && np.y < 25)
        {
            engine.clipboard_entity = true;
            engine.l_mouse_pressed = false;
            engine.entity = entities[selected_layer][i];
            entities[selected_layer].erase(entities[selected_layer].begin() + i);
            return true;
        }
    }
    if(!hasTileset) return false;
    if(pos.x < 0 || pos.y < 0) return false;
    pos.x /= tilesize.x;
    pos.y /= tilesize.y;
    if(pos.x >= (int32_t)size.x || pos.y >= (int32_t)size.y) return false;
    switch(engine.pen_mode)
    {
        case 0: tiles[selected_layer][size.y*pos.x+pos.y] = engine.current_tile; break;
        case 1: fill(selected_layer, pos.x, pos.y, tiles[selected_layer][size.y*pos.x+pos.y], engine.current_tile); break;
        default: break;
    }
    return true;
}

void Map::fill(int32_t l, int32_t x, int32_t y, int16_t t, int16_t n)
{
    if(l < 0 || l >= (int32_t)tiles.size()) return;
    if(t == n) return;
    std::queue<sf::Vector2i> q;

    if(tiles[l][size.y*x+y] == t)
    {
        tiles[l][size.y*x+y] = n;
        q.push(sf::Vector2i(x, y));
    }

    while(!q.empty())
    {
        sf::Vector2i pnt = q.front();
        q.pop();
        sf::Vector2i adj[4];
        adj[0].x = pnt.x;   adj[0].y = pnt.y-1;   // up
        adj[1].x = pnt.x+1; adj[1].y = pnt.y;     // right
        adj[2].x = pnt.x;   adj[2].y = pnt.y+1;   // down
        adj[3].x = pnt.x-1; adj[3].y = pnt.y;     // left
        for(int i = 0; i < 4; i++)
        {
            if(adj[i].x < 0 || adj[i].x >= (int32_t)size.x ||
                adj[i].y < 0 || adj[i].y >= (int32_t)size.y)
            continue;

            if(tiles[l][size.y*adj[i].x+adj[i].y] == t)
            {
                tiles[l][size.y*adj[i].x+adj[i].y] = n;
                q.push(adj[i]);
            }
        }
    }
}

//#############################################################################

Button::Button()
{

}

Button::Button(sf::Vector2i s, std::string sprite_file)
{
    box = s;
    if(text.loadFromFile(sprite_file))
        sprite.setTexture(text);
}

Button::~Button()
{

}

bool Button::click(sf::Vector2i p)
{
    if(p.x >= pos.x &&
       p.y >= pos.y &&
       p.x < box.x + pos.x &&
       p.y < box.y + pos.y) return true;
    return false;
}

void Button::draw()
{
    sprite.setPosition(pos.x, pos.y);
    engine.draw(&sprite);
}

TextDial::TextDial(std::string t, std::string* ptr)
{
    title = t;
    str = ptr;
    text.setFont(engine.font);
    text.setCharacterSize(10);
    ok = new Button(sf::Vector2i(48, 24), "editor_hud/dial_ok.png");
    cancel = new Button(sf::Vector2i(48, 24), "editor_hud/dial_cancel.png");
    got = false;
    forced = false;
}

TextDial::~TextDial()
{
    delete ok;
    delete cancel;
}

bool TextDial::click(sf::Vector2i p)
{
    if(ok->click(p) || forced)
    {
        got = true;
        return true;
    }
    else if(cancel->click(p)) return true;
    return false;
}

void TextDial::draw()
{
    box.setPosition(pos.x, pos.y);
    box.setFillColor(sf::Color::Black);
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setSize(sf::Vector2f(200, 100));
    engine.draw(&box);

    text.setPosition(pos.x+5, pos.y+5);
    text.setString(title);
    engine.draw(&text);

    text.setPosition(pos.x+5, pos.y+35);
    if(str) text.setString(*str);
    else text.setString("");
    engine.draw(&text);

    ok->setPosition(pos.x+20, pos.y+70);
    ok->draw();
    cancel->setPosition(pos.x+80, pos.y+70);
    cancel->draw();
}

bool TextDial::gotInput()
{
    return got;
}

void TextDial::forceOk()
{
    forced = true;
}
