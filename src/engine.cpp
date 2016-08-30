#include "engine.hpp"
#include "mlib/mlib.hpp"
#include "usermessage.hpp"
#include "version.h"

#include "mlib/mrand.hpp"
#include <ctime>

#ifdef WINDOWS_VERSION
#define GAME_SAVE_FOLDER "test\\"
#else
#define GAME_SAVE_FOLDER ".test/"
#endif

#define ABS(x) ((x)<0 ? -(x) : (x))
#define SGN(x) ((x)<0 ? -1 : 1)

#define ENGINE_SFML_VERSION "SFML v2.3.2"
#define ENGINE_SIZE_LIMIT 1024
#define ENGINE_DEFAULT_FONT "font.ttf"
#define ENGINE_UNBIND_KEY 999

#define USE_SETTING_DATA
#include "game/data.hpp"
#include "game/menu.hpp"
#include "game/variable.hpp"

#include <SFML/System/Mutex.hpp>
extern sf::Mutex mutex;

Engine::Engine()
{
    mrand::setSeed(time(NULL));
    state = INIT;
    Out.resetTimestamp();
    Out.useTimestamp(true);
    #ifdef DEBUG_BUILD
    Out.setFile("log.txt", true);
    Out.setOutput(true, true);
    Out = "[DEBUG ENABLED]\n";
    Out = std::string("SF2DEngine v") + AutoVersion::STATUS + " " + AutoVersion::FULLVERSION_STRING + " (Build:" + mlib::uint2str(AutoVersion::BUILDS_COUNT) + ")\n";
    Out = "Debug Build - " + std::string(ENGINE_SFML_VERSION) + "\n";
    #else
    #ifdef _USE_SAVE_
    sv = mlib::getSavePath() + GAME_SAVE_FOLDER;
    Out = "Save directory: " + sv + "\n";
    #endif
    if(!mlib::dirExist(sv)) mlib::makeDir(sv);
    Out.setFile(/*sv + */"log.txt", true);
    Out.setOutput(false, true);
    Out = std::string("SF2DEngine v") + AutoVersion::STATUS + " " + AutoVersion::FULLVERSION_STRING + " (Build:" + mlib::uint2str(AutoVersion::BUILDS_COUNT) + ")\n";
    Out = "Release Build - " + std::string(ENGINE_SFML_VERSION) + "\n";
    #endif

    // module listing
    Out = "======= Module state =======\n";
    Out.useTimestamp(false);
    #ifdef _USE_ALARM_
    Out = "Alarm Module\t\t\t: Ok\n";
    #else
    Out = "Alarm Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_ENTITY_
    Out = "Entity Module\t\t\t: Ok\n";
    #else
    Out = "Entity Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_FILESYSTEM_
    Out = "Filesystem Module\t\t: Ok\n";
    #else
    Out = "Filesystem Module\t\t: Not compiled\n";
    #endif
    #ifdef _USE_FONT_
    Out = "Font Module\t\t\t: Ok\n";
    #else
    Out = "Font Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_INIFILE_
    Out = "ini File Module\t\t\t: Ok\n";
    #else
    Out = "ini File Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_INPUT_
    Out = "Input Module\t\t\t: Ok\n";
    #else
    Out = "Input Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_LANG_
    Out = "Language Module\t\t\t: Ok\n";
    #else
    Out = "Language Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_MAP_
    Out = "Map Module\t\t\t: Ok\n";
    #else
    Out = "Map Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_MENU_
    Out = "Menu Module\t\t\t: Ok\n";
    #else
    Out = "Menu Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_MUSIC_
    Out = "Music Module\t\t\t: Ok\n";
    #else
    Out = "Music Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_SAVE_
    Out = "Save Module\t\t\t: Ok\n";
    #else
    Out = "Save Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_SOUND_
    Out = "Sound Module\t\t\t: Ok\n";
    #else
    Out = "Sound Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_TEXTURE_
    Out = "Texture Module\t\t\t: Ok\n";
    #else
    Out = "Texture Module\t\t\t: Not compiled\n";
    #endif
    #ifdef _USE_NETWORK_
    Out = "Network Module\t\t\t: Ok\n";
    #else
    Out = "Network Module\t\t\t: Not compiled\n";
    #endif
    Out.useTimestamp(true);

    // ini file loading
    #ifdef _USE_INIFILE_
    //#ifdef DEBUG_BUILD
    if(!inifile.load("config.ini"))
    //#else
    //if(!inifile.load(sv + "config.ini"))
    //#endif
    {
        Out = "Engine: failed to load \"config.ini\". Using default settings...\n";
        init_ini();
    }
    #endif

    // filesystem
    #ifdef _USE_FILESYSTEM_
    files.useCryto(true);
    #endif

    bool mod_enabled;
    #ifdef _USE_INIFILE_
    if(inifile.exist("MOD_ENABLED", "ENGINE")) mod_enabled = inifile.getBoolData("MOD_ENABLED", "ENGINE");
    else mod_enabled = false;
    #else
    mod_enabled = false;
    #endif

    std::string mod_folder;
    #ifdef _USE_INIFILE_
    if(inifile.exist("MOD_FOLDER", "ENGINE")) mod_folder = inifile.getStrData("MOD_FOLDER", "ENGINE");
    else mod_folder = "";
    #else
    mod_folder = "";
    #endif

    threads.run(2, mod_enabled, mod_folder);

    // graphic
    sizeLimit = sf::Texture::getMaximumSize();

    #ifdef _USE_INIFILE_
    if(inifile.exist("WINDOW_NAME", "ENGINE")) wintitle = inifile.getStrData("WINDOW_NAME", "ENGINE");
    else wintitle = "No title";
    #else
    wintitle = "No title";
    #endif

    view[0].setCenter(320, 240);
    rTex = &tex[0];
    color[0] = sf::Color(255, 255, 255, 255);
    color[1] = sf::Color(255, 255, 255, 255);
    refreshVideoMode();

    #ifdef _USE_INIFILE_
    if(inifile.exist("LAST_VIDEOMODE", "GAME_SETTINGS")) currentVideoMode = inifile.getIntData("LAST_VIDEOMODE", "GAME_SETTINGS");
    else currentVideoMode = - 1;
    #else
    currentVideoMode = -1;
    #endif

    winmode = -1;

    #ifdef _USE_INIFILE_
    if(inifile.exist("FPS_LIMIT", "GAME_SETTINGS")) winfps = inifile.getIntData("FPS_LIMIT", "GAME_SETTINGS");
    else winfps = 30;
    #else
    winfps = 30;
    #endif

    focus = true;

    // others
    #ifdef _USE_INIFILE_
    if(inifile.exist("MASTER_VOLUME", "GAME_SETTINGS")) masterVolume = inifile.getIntData("MASTER_VOLUME", "GAME_SETTINGS");
    else masterVolume = 100;
    #else
    masterVolume = 100;
    #endif

    if(masterVolume > 100 || masterVolume < 0) masterVolume = 100;

    #ifdef _USE_SOUND_
    dummy_soundbuffer = new sf::SoundBuffer();
    #endif

    sprite[0].setPosition(0, 0);
    sprite[1].setPosition(0, 0);
    sprite[0].setTexture(tex[0].getTexture());
    sprite[1].setTexture(tex[1].getTexture());

    #ifdef _USE_INPUT_
    input.enableMouse(true);
    input.init();
    load_ini();
    input.pad_auto_connect();
    input.update();
    #endif

    game_pause = false;
    #ifdef _USE_MAP_
    mapspace = nullptr;
    map = nullptr;
    #endif
    change_map = false;

    game_loop_maxupdates = 30;
    game_loop_updaterate = (1000.f / (float)(game_loop_maxupdates));
    Out = "Engine: Max updates=" + mlib::int2str(game_loop_maxupdates) + " / Update rate=" + mlib::float2str(game_loop_updaterate) + "ms\n";
}

Engine::~Engine()
{
    if(state != STOPPED)
    {
        mutex.lock();
        Out = "WARNING: Engine forcibly stopped. Attempting a normal exit...\n";
        mutex.unlock();
        term();
    }

    #ifdef _USE_SOUND_
    delete dummy_soundbuffer;
    #endif

    Out = "Engine stopped\n";
    Out = "Running time : " + mlib::uint2str(Out.t() / 3600) + " hours "
                                + mlib::uint2str(Out.t()  / 60) + " minutes "
                                + mlib::uint2str(Out.t()  % 60) + " seconds\n";
}

int32_t Engine::run()
{
    init();
    setMap("mapStartup");
    while(state == RUNNING)
    {
        update();
        render();
        #ifdef DEBUG_BUILD
        debug_timer += debug_clock.restart();
        if(debug_timer >= sf::microseconds(1000000))
        {
            debug_timer -= sf::microseconds(1000000);
            dStr[0] = "*FPS(" + mlib::int2str(dVar[0]) + ")\nTick(" + mlib::int2str(dVar[1]) + ")*";
            dVar[0] = 0;
            dVar[1] = 0;
            hud->updateDebug();
        }
        #endif
    }
    term();
    return 0;
}

void Engine::init()
{
    Out = "Engine: Waiting for filesystem...\n";
    mutex.lock();
    Out = "Engine: Filesystem thread finished. Checking...\n";
    mutex.unlock();
    #ifdef DEBUG_BUILD
    Out = "Engine: Debug mode is enabled. Check is skipped\n";
    #else
    if(files.getByteSize() == 0)
    {
        Out = "Engine: \"data.pack\" isn't found or valid\n";
        UserMessage::msg(L"Can't create the filesystem.\n\"data.pack\" is missing or invalid");
        exit(0);
    }
    #endif
    Out = "Engine: Initialization\n";
    // checking graphic card limit
    Out = "Graphic card size limit: " + mlib::uint2str(sizeLimit) + "\n";
    if(sizeLimit < ENGINE_SIZE_LIMIT)
    {
        Out = "Inferior to the minimum. Exit...\n";
        UserMessage::msg(L"Texture's size limit is too low.");
        exit(0);
    }

    // language
    #ifdef _USE_LANG_
    langs.setFolder("lang");
    if(!langs.loadFromMemory("lang/language.config"))
    {
        Out = "Engine: \"lang/language.config\" loading failed\n";
        state = STOPPING;
        return;
    }
    else Out = "Engine: " + mlib::uint2str(langs.getLangCount()) + " language(s) found\n";
    #endif

    #ifdef _USE_LANG_
    #ifdef _USE_INIFILE_
    if(inifile.exist("LANGUAGE", "GAME_SETTINGS") && langs.loadLang(inifile.getStrData("LANGUAGE", "GAME_SETTINGS")))
    {
        Out = "Engine: " + langs.getCurrentLangString() + " language file loaded\n";
    }
    else if(langs.loadLang(0))
    #else
    if(langs.loadLang(0))
    #endif
    {
        Out = "Engine: First language file loaded\n";
    }
    else
    {
        Out = "Engine: No default language ?\n";
        state = STOPPING;
        return;
    }
    #endif

    // font
    #ifdef _USE_FONT_
    fonts.setCurrentFont(ENGINE_DEFAULT_FONT);
    if(!fonts.useCurrentFont())
    {
        Out = "Engine: Default font \"" + std::string(ENGINE_DEFAULT_FONT) + "\"not found\n";
        state = STOPPING;
        return;
    }
    else Out = "Engine: Default font \"" + std::string(ENGINE_DEFAULT_FONT) + "\" loaded\n";
    #endif

    // init save & global variable
    initDungeonData();

    initMenuData();
    Menu::initMenuScript();
    RichText::initializeColors();

    #ifdef _USE_SAVE_
    if(!saves.init("save", "test_save", 3))
    {
        Out = "WARNING: Save system initializion failed\n";
    }
    #ifdef DEBUG_BUILD
    else
    {
        std::vector<SlotState> slotStates = saves.getSlotState();
        for(size_t i = 0; i < slotStates.size(); ++i)
            Out = "Save slot [" + mlib::int2str(i) + "]: " + mlib::int2str(slotStates[i].used) + " > " + slotStates[i].header + "\n";
    }
    #endif
    if(!saves.loadSystem())
    {
        Out = "Failed to load the system save\n";
        initGlobaleVariable();
        updateGlobaleString();
        updateSystemString();
    }
    else Out = "System save loaded\n";
    #endif

    // hud
    showHUD = false;
    hud = new HUD();
    hud->set(showHUD);
    hud->update();

    // others
    #ifdef DEBUG_BUILD
    debug_clock.restart();
    debug_timer = sf::Time::Zero;
    dVar[0] = 0;
    dVar[1] = 0;
    hud->updateDebug();
    #endif

    // start
    system_resize = false;
    #ifdef _USE_INIFILE_
    if(inifile.exist("WIN_MODE", "GAME_SETTINGS"))
    {
        int32_t tmp = inifile.getIntData("WIN_MODE", "GAME_SETTINGS");
        if(tmp < 0 || tmp > 2) changeWindowMode(0);
        else changeWindowMode(tmp);
    }
    else changeWindowMode(0);
    #else
    changeWindowMode(0);
    #endif

    setFPSLimit(winfps);

    elapsedTime.restart();
    tickTime.restart();
    elapsedTickTime = sf::Time::Zero;
    anUpdateClock.restart();
    anUpdateNext = anUpdateClock.getElapsedTime().asMilliseconds();
    state = RUNNING;
    Out = "Engine ready\n";
}

void Engine::stop()
{
    state = STOPPING;
}

void Engine::term()
{
    if(state == STOPPED) return;

    // clear remaining...
    threads.clear();
    if(win.isOpen()) win.close();
    #ifdef _USE_MAP_
    if(mapspace) delete mapspace;
    #endif

    // delete hud
    delete hud;
    hud = NULL;

    // update save data
    #ifdef _USE_SAVE_
    saves.saveSystem();
    #endif
    refresh_ini();

    // stop alarms
    #ifdef _USE_ALARM_
    for(Alarm* &i: alarms) delete i;
    alarms.clear();
    #endif

    // clear datas
    #ifdef _USE_MENU_
    menus.clear();
    #endif
    #ifdef _USE_FONT_
    fonts.clear();
    #endif
    #ifdef _USE_LANG_
    langs.clear();
    #endif
    #ifdef _USE_MUSIC_
    musics.clear();
    #endif
    #ifdef _USE_SOUND_
    sounds.clear();
    #endif
    #ifdef _USE_TEXTURE_
    textures.clear();
    #endif

    // update ini file
    #ifdef _USE_INIFILE_
    #ifdef DEBUG_BUILD
    if(!inifile.save("config.ini"))
    #else
    if(!inifile.save(sv + "config.ini"))
    #endif
        Out = "Engine: failed to save \"config.ini\"\n";
    #endif // _USE_INIFILE_

    state = STOPPED;
}

bool Engine::hasFocus() const
{
    return focus;
}

bool Engine::isRunning() const
{
    return (state == RUNNING);
}

sf::Time Engine::getElapsedFrameTime() const
{
    return elapsedTime.getElapsedTime();
}

sf::Time Engine::getElapsedTickTime() const
{
    return elapsedTickTime;
}

void Engine::draw(const sf::Drawable *ptr)
{
    if(!ptr) return;
    rTex->draw(*ptr);
}

void Engine::newAlarm(int32_t id, sf::Time t)
{
    #ifdef _USE_ALARM_
    alarms.push_back(new Alarm(id, t));
    #endif
}

void Engine::setMap(const std::string &name, int32_t type)
{
    if(change_map) return;
    change_map = true;
    next_map = name;
    threads.run(1, type, name);
}

void Engine::refreshVideoMode()
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
    Out = "Refreshing video mode list...\n";
    for(size_t i = 0; i < modes.size(); ++i)
    {
        Out = "Mode (" + mlib::uint2str(i) + "): " + mlib::uint2str(modes[i].width) + "x" + mlib::uint2str(modes[i].height) + "\n";
    }
    #endif
}

const std::vector<sf::VideoMode>& Engine::getVideoModeList()
{
    return modes;
}

void Engine::changeVideoMode(int32_t m)
{
    if(winmode != 1 || m == currentVideoMode || m < 0 || m >= (int32_t)modes.size()) return;
    currentVideoMode = m;
    create(modes[currentVideoMode], wintitle, sf::Style::Fullscreen);
    Out = "Engine: Fullscreen resolution set to (" + mlib::int2str(modes[currentVideoMode].width) + "x" + mlib::int2str(modes[currentVideoMode].height) + ")\n";
    system_resize = true;
}

int32_t Engine::getCurrentVideoMode()
{
    return currentVideoMode;
}

int32_t Engine::getDesktopModeID()
{
    sf::VideoMode desk = sf::VideoMode().getDesktopMode();
    for(size_t i = 0; i < modes.size(); ++i)
        if(modes[i].width == desk.width && modes[i].height == desk.height) return i;
    return 0;
}

void Engine::changeWindowMode(int8_t m)
{
    if(m == winmode || m < 0 || m > 2) return;
    switch(m)
    {
        case 0:
        {
            winmode = 0;
            currentVideoMode = getDesktopModeID();
            create(sf::VideoMode(640, 480), wintitle);
            Out = "Engine: Windowed mode\n";
            break;
        }
        case 1:
        {
            winmode = 1;
            if(currentVideoMode < 0 || currentVideoMode >= (int32_t)modes.size()) currentVideoMode = 0;
            create(modes[currentVideoMode], wintitle, sf::Style::Fullscreen);
            Out = "Engine: Fullscreen mode (" + mlib::int2str(modes[currentVideoMode].width) + "x" + mlib::int2str(modes[currentVideoMode].height) + ")\n";
            break;
        }
        case 2:
        {
            if(winmode == 1) create(sf::VideoMode(640, 480), wintitle);
            winmode = 2;
            currentVideoMode = getDesktopModeID();
            create(modes[currentVideoMode], wintitle, sf::Style::None);
            Out = "Engine: Borderless mode\n";
            break;
        }
    }
    system_resize = true;
}

int8_t Engine::getWindowMode()
{
    return winmode;
}

void Engine::setFPSLimit(uint32_t fps)
{
    winfps = fps;
    if(winfps < 30)
    {
        Out = "FPS limit too low. Setting to 30...\n";
        winfps = 30;
    }
    else Out = "FPS limit set to " + mlib::uint2str(winfps) + "\n";
    win.setFramerateLimit(winfps);
}

uint32_t Engine::getFPSLimit()
{
    return winfps;
}

sf::Vector2f Engine::getGameViewCenter() const
{
    return view[0].getCenter();
}

sf::IntRect Engine::getGameBounds() const
{
    return sf::IntRect(view[0].getCenter().x-view[0].getSize().x/2,
                       view[0].getCenter().y-view[0].getSize().y/2,
                       view[0].getSize().x,
                       view[0].getSize().y);
}

sf::IntRect Engine::getHUDBounds() const
{
    return sf::IntRect(view[1].getCenter().x-view[1].getSize().x/2,
                       view[1].getCenter().y-view[1].getSize().y/2,
                       view[1].getSize().x,
                       view[1].getSize().y);
}

void Engine::moveGameView(int32_t x, int32_t y)
{
    view[0].move(x, y);
}

void Engine::setGameViewCenter(int32_t x, int32_t y)
{
    view[0].setCenter(x, y);
}

void Engine::setScreenColor(sf::Color c, bool high)
{
    if(high) color[1] = c;
    else color[0] = c;
}

void Engine::updateHUDValue(int32_t id)
{
    switch(id)
    {
        default: return;
    }
}

void Engine::loadSetting()
{
    settWinS = winmode;
    settVMode = currentVideoMode;
    settFPS = winfps;
    settVol = masterVolume;
    #ifdef _USE_LANG_
    settLang = engine.langs.getCurrentLang();
    #endif
}

void Engine::saveSetting()
{
    setFPSLimit(settFPS);
    masterVolume = settVol;
    #ifdef _USE_MUSIC_
    musics.updateVolume();
    #endif
    Out = "Engine: Master volume set to " + mlib::int2str(masterVolume) + "\n";
}

void Engine::revertSetting()
{
    Out = "Engine: cancelling settings...\n";
    changeWindowMode(settWinS);
    changeVideoMode(settVMode);
    winmode = settWinS;
    setFPSLimit(winfps);
    #ifdef _USE_LANG_
    engine.langs.loadLang(settLang);
    #endif
}

void Engine::update()
{
    mutex.lock();
    static int32_t anUpdates;
    static int32_t anUpdateTime;
    if(!win.isOpen())
    {
        state = STOPPING;
        mutex.unlock();
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
                {
                    state = STOPPING;
                    mutex.unlock();
                    return;
                }
                case sf::Event::Resized:
                    Out = "Window resized\n";
                    resize();
                    break;
                case sf::Event::LostFocus:
                    Out = "Window lost focus\n";
                    focus = false;
                    break;
                case sf::Event::GainedFocus:
                    Out = "Window gained focus\n";
                    focus = true;
                    break;
                case sf::Event::JoystickConnected:
                    Out = "Joystick connected\n";
                    #ifdef _USE_INPUT_
                    input.pad_connect(event);
                    #endif
                    break;
                case sf::Event::JoystickDisconnected:
                    Out = "Joystick disconnected\n";
                    #ifdef _USE_INPUT_
                    input.pad_disconnect(event);
                    #endif
                    break;
                case sf::Event::KeyPressed: // TEST
                    if(settChangingKey)
                    {
                        #ifdef _USE_INPUT_
                        input.bindKey(settKeyCode, event.key.code);
                        #endif
                        settChangingKey = false;
                        #ifdef _USE_MENU_
                        menus.delAndAdd(9);
                        menus.disableInput();
                        #endif
                    }
                    //if(event.key.code == sf::Keyboard::F1) game_pause = !game_pause;
                    break;
                case sf::Event::MouseButtonPressed:
                    if(settChangingKey)
                    {
                        #ifdef _USE_INPUT_
                        input.bindMouse(settKeyCode, event.mouseButton.button);
                        #endif
                        settChangingKey = false;
                        #ifdef _USE_MENU_
                        menus.delAndAdd(9);
                        menus.disableInput();
                        #endif
                    }
					break;
                case sf::Event::JoystickButtonPressed:
                    #ifdef _USE_INPUT_
                    if(settChangingKey && event.joystickButton.joystickId == (uint32_t)input.get_pad_id())
                    {
                        input.bindPadButton(settKeyCode, JOYSTICK_SETTING_TYPE + event.joystickButton.button);
                        settChangingKey = false;
                        #ifdef _USE_MENU_
                        menus.delAndAdd(9);
                        menus.disableInput();
                        #endif
                    }
                    #endif
					break;
                case sf::Event::JoystickMoved:
                    #ifdef _USE_INPUT_
                    if(settChangingKey && event.joystickButton.joystickId == (uint32_t)input.get_pad_id())
                    {
                        if(ABS(event.joystickMove.position) > JOYSTICK_DEAD_ZONE)
                        {
                            input.bindPadButton(settKeyCode, (JOYSTICK_SETTING_OFFSET + event.joystickMove.axis) * SGN(event.joystickMove.position));
                            settChangingKey = false;
                            #ifdef _USE_MENU_
                            menus.delAndAdd(9);
                            menus.disableInput();
                            #endif
                        }
                    }
                    #endif
					break;
                default:
                    break;
            }
        }

        #ifdef _USE_INPUT_
        if(focus) input.update(&win);
        else input.reset();
        #endif

        #ifdef _USE_MENU_
        if(!game_pause && !change_map)
        {
            menus.update(); // update the menu
            menus.enableInput();
            if(!menus.gamePaused())
            {
                #ifdef _USE_ALARM_
                for(size_t i = 0; i < alarms.size(); ++i) // update alarms
                {
                    if(!alarms[i]->update()) // remove if the alarm run its code
                    {
                        delete alarms[i];
                        alarms.erase(alarms.begin()+i);
                        --i;
                    }
                }
                #endif
                mutex.unlock();
                #ifdef _USE_MAP_
                if(map) map->updateMap(); // update the map and its entities
                #endif
                mutex.lock();
            }
        }
        #endif

        mutex.unlock();
        // map change
        #ifdef _USE_MAP_
        if(change_map)
        {
            mutex.lock();
            if(mapspace)
            {
                if(!mapspace->isLoaded())
                {
                    if(mapspace->loadFailed())
                    {
                        Out = "Can't load the next map: \"" + next_map + "\"\n";
                        UserMessage::msg(L"Error: failed to load the next map");
                        state = STOPPING;
                        cdelete(mapspace);
                        change_map = false;
                    }
                }
                else if(!mapspace->loadFailed())
                {
                    change_map = false;
                    delete map;
                    map = mapspace;
                    mapspace = nullptr;
                    Out = "Engine: Current map modified\n";
                }
            }
            mutex.unlock();
        }
        #endif
        // garbage collection
        garbageCollection();

        mutex.lock();
        // Compute the next appropriate UpdateFixed time
        anUpdateNext += game_loop_updaterate;
        // update tick clock
        elapsedTickTime = tickTime.getElapsedTime();

        #ifdef DEBUG_BUILD
        dVar[1]++;
        #endif

        tickTime.restart();
    }
    mutex.unlock();
}

void Engine::render()
{
    mutex.lock();
    if(state == STOPPING)
    {
        mutex.unlock();
        return;
    }

    // clear
    tex[0].clear(sf::Color::Black);
    tex[1].clear(sf::Color::Transparent);

    // view A
    rTex = &tex[0];
    tex[0].setView(view[0]);
    #ifdef _USE_MAP_
    if(map) map->drawMap();
    #endif

    // view B
    rTex = &tex[1];
    tex[1].setView(view[1]);
    hud->draw();
    #ifdef _USE_MENU_
    menus.draw();
    #endif

    // refresh texture
    tex[0].display();
    tex[1].display();

    // apply color
    sprite[0].setColor(color[0]);
    sprite[1].setColor(color[1]);

    // refresh window
    win.clear();
    win.setView(view[1]);
    win.draw(sprite[0]);
    win.draw(sprite[1]);
    win.display();

    #ifdef DEBUG_BUILD
    dVar[0]++;
    #endif

    elapsedTime.restart();
    mutex.unlock();
}

void Engine::resize()
{
    bool limited = false;
    sf::Vector2f s(win.getSize().x, win.getSize().y);
    if(!system_resize)
    {
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
    }
    else system_resize = false;

    view[0].setSize(s);
    view[1].setSize(s);
    view[1].setCenter(s.x/2, s.y/2);

    tex[0].create(s.x, s.y);
    tex[1].create(s.x, s.y);

    sprite[0].setTextureRect(sf::IntRect(0, 0, s.x, s.y));
    sprite[1].setTextureRect(sf::IntRect(0, 0, s.x, s.y));

    hud->update();
}

void Engine::create(sf::VideoMode mode, const sf::String &title, sf::Uint32 style, const sf::ContextSettings &settings)
{
    sizeLimit = sf::Texture::getMaximumSize();
    if(mode.width > sizeLimit) mode.width = sizeLimit;
    if(mode.height > sizeLimit) mode.height = sizeLimit;
    win.create(mode, title, style, settings);
    win.setFramerateLimit(winfps);
    wintitle = title;
    resize();
}

void Engine::garbageCollection()
{
    static int32_t call_count = -1;
    ++call_count;
    if(call_count == game_loop_maxupdates)
    {
        // thread cleaning
        threads.garbageCollection();
        mutex.lock();
        // update audio
        #ifdef _USE_MUSIC_
        musics.update();
        #endif
        #ifdef _USE_SOUND_
        sounds.update();
        #endif
        // file management
        #ifdef _USE_FONT_
        fonts.garbageCollection();
        #endif
        #ifdef _USE_TEXTURE_
        textures.garbageCollection();
        #endif
        #ifdef _USE_MUSIC_
        musics.garbageCollection();
        #endif
        #ifdef _USE_SOUND_
        sounds.garbageCollection();
        #endif
        call_count = 0;
        mutex.unlock();
    }
}

void Engine::init_ini()
{
    #ifdef _USE_INIFILE_
    inifile.editStrData("WINDOW_NAME", "Game test", "ENGINE");
    inifile.editBoolData("MOD_ENABLED", 1, "ENGINE");
    inifile.editStrData("MOD_FOLDER", "patch", "ENGINE");
    inifile.editIntData("FPS_LIMIT", 30, "GAME_SETTINGS");
    inifile.editIntData("WIN_MODE", 0, "GAME_SETTINGS");
    inifile.editIntData("LAST_VIDEOMODE", -1, "GAME_SETTINGS");
    inifile.editIntData("MASTER_VOLUME", 100, "GAME_SETTINGS");
    inifile.editStrData("LANGUAGE", "English", "GAME_SETTINGS");
    #endif
}

void Engine::load_ini()
{
    #ifdef _USE_INIFILE_
    #ifdef _USE_INPUT_
    int tmp;
    for(int8_t i = 0; i < KCOUNT; ++i)
    {
        if(inifile.exist("K"+mlib::int2str(i)+"K", "KEYBIND"))
        {
            tmp = inifile.getIntData("K"+mlib::int2str(i)+"K", "KEYBIND");
            if(tmp == ENGINE_UNBIND_KEY) input.unbingKey(i);
            else input.bindKey(i, static_cast<sf::Keyboard::Key>(tmp));
        }
        if(inifile.exist("K"+mlib::int2str(i)+"M", "KEYBIND"))
        {
            tmp = inifile.getIntData("K"+mlib::int2str(i)+"M", "KEYBIND");
            if(tmp == ENGINE_UNBIND_KEY) input.unbingMouse(i);
            else input.bindMouse(i, static_cast<sf::Mouse::Button>(tmp));
        }
        if(inifile.exist("K"+mlib::int2str(i)+"B", "KEYBIND"))
        {
            tmp = inifile.getIntData("K"+mlib::int2str(i)+"B", "KEYBIND");
            if(tmp == ENGINE_UNBIND_KEY) input.unbingPadButton(i);
            else input.bindPadButton(i, tmp);
        }
    }
    #endif
    #endif
}

void Engine::refresh_ini()
{
    #ifdef _USE_INIFILE_
    inifile.editStrData("VERSION_PLAYED", AutoVersion::FULLVERSION_STRING, "ENGINE");
    inifile.editStrData("WINDOW_NAME", "Game test", "ENGINE");
    #ifdef _USE_FILESYSTEM_
    inifile.editBoolData("MOD_ENABLED", files.isModEnabled(), "ENGINE");
    inifile.editStrData("MOD_FOLDER", files.getModFolder(), "ENGINE");
    #endif
    inifile.editIntData("FPS_LIMIT", winfps, "GAME_SETTINGS");
    inifile.editIntData("WIN_MODE", winmode, "GAME_SETTINGS");
    inifile.editIntData("LAST_VIDEOMODE", currentVideoMode, "GAME_SETTINGS");
    inifile.editIntData("MASTER_VOLUME", masterVolume, "GAME_SETTINGS");
    #ifdef _USE_LANG_
    inifile.editStrData("LANGUAGE", langs.getCurrentLangString(), "GAME_SETTINGS");
    #endif

    #ifdef _USE_INPUT_
    for(int8_t i = 0; i < KCOUNT; ++i)
    {
        inifile.erase("K"+mlib::int2str(i)+"K", "KEYBIND");
        inifile.erase("K"+mlib::int2str(i)+"M", "KEYBIND");
        inifile.erase("K"+mlib::int2str(i)+"B", "KEYBIND");

        if(input.keyIsBinded(i)) inifile.editIntData("K"+mlib::int2str(i)+"K", input.getKeyID(i), "KEYBIND");
        else inifile.editIntData("K"+mlib::int2str(i)+"K", ENGINE_UNBIND_KEY, "KEYBIND");

        if(input.mouseIsBinded(i)) inifile.editIntData("K"+mlib::int2str(i)+"M", input.getMouseID(i), "KEYBIND");
        else inifile.editIntData("K"+mlib::int2str(i)+"M", ENGINE_UNBIND_KEY, "KEYBIND");

        if(input.padButtonIsBinded(i)) inifile.editIntData("K"+mlib::int2str(i)+"B", input.getPadButtonID(i), "KEYBIND");
        else inifile.editIntData("K"+mlib::int2str(i)+"B", ENGINE_UNBIND_KEY, "KEYBIND");
    }
    #endif
    #endif
}
