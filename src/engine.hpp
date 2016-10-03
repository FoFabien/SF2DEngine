#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Mutex.hpp>
#include <vector>

#include "plugin/file/filesystem.hpp"
#include "plugin/file/inifile.hpp"
#include "plugin/memory/managerfont.hpp"
#include "plugin/memory/managerlang.hpp"
#include "plugin/memory/managermusic.hpp"
#include "plugin/memory/managershader.hpp"
#include "plugin/memory/managersound.hpp"
#include "plugin/memory/managertexture.hpp"
#include "plugin/memory/managerthread.hpp"
#include "plugin/memory/managermenu.hpp"
#include "plugin/file/save.hpp"

#include "game/hud.hpp"

#include "plugin/system/input.hpp"
#include "plugin/drawable/richtext.hpp"
#include "plugin/game/map.hpp"
#include "plugin/system/alarm.hpp"
#include "plugin/system/network.hpp"

class Engine
{
    public:
        Engine();
        ~Engine();

        // -----------------------------------------------
        // public function
        // -----------------------------------------------
        // Engine
        // -----------------------------------------------
        int32_t run();
        // called by run :
        void init();                                                // need to be called first
        void stop();                                                // change the engine state to "STOPPING"
        void term();                                                // need to be called last
        // engine state :
        bool hasFocus() const;                                      // check if the window has focus
        bool isRunning() const;                                     // check if the state is set to "RUNNING"
        // speed & time
        sf::Time getElapsedFrameTime() const;                       // elapsed time since the last render
        sf::Time getElapsedTickTime() const;                        // elapsed time since the last tick
        // others
        void draw(const sf::Drawable *ptr, const sf::RenderStates &states=sf::RenderStates::Default);                         // to draw something on the screen
        void newAlarm(int32_t id, sf::Time t);                      // to set an alarm
        void setMap(const std::string &name, int32_t type = 0);     // change the map. type is used for user defined type of map
        void mutexLock();                                           // lock engine's mutex
        void mutexUnlock();                                         // and unlock
        // -----------------------------------------------
        // Map
        // -----------------------------------------------
        #ifdef _USE_MAP_
        Map* mapspace;                                              // pointer to store a loading map
        #endif
        bool change_map;                                            // if true, next_map will be loaded
        std::string next_map;                                       // name of the map to load
        int32_t map_type;                                           // 0 is default, others are for custom map
        // -----------------------------------------------
        // Video
        // -----------------------------------------------
        // info
        void refreshVideoMode();                                    // refresh video mode list
        const std::vector<sf::VideoMode>& getVideoModeList();       // return the video mode list
        // video mode
        void changeVideoMode(int32_t m);                            // set the video mode (id has to be valid)
        int32_t getCurrentVideoMode();                              // return the id of the current video mode
        int32_t getDesktopModeID();                                 // return the id of the video mode used for borderless fullscreen mode
        // window mode
        void changeWindowMode(int8_t m);                            // set the window mode (windowed = 0, fullscreen = 1, borderless = 2)
        int8_t getWindowMode();                                     // get the current window mode (windowed = 0, fullscreen = 1, borderless = 2)
        // framerate
        void setFPSLimit(uint32_t fps);                             // set the framerate limit
        uint32_t getFPSLimit();                                     // return the framerate limit
        // -----------------------------------------------
        // Camera
        // -----------------------------------------------
        sf::Vector2f getGameViewCenter() const;                     // return the center of the lower view
        sf::IntRect getGameBounds() const;                          // return the bounds of the lower view
        sf::IntRect getHUDBounds() const;                           // return the bounds of the higher view
        void moveGameView(int32_t x, int32_t y);                    // move the lower view
        void setGameViewCenter(int32_t x, int32_t y);               // set the lower view center
        void setScreenColor(sf::Color c, bool high);                // colorize a view
        // hud
        void updateHUDValue(int32_t id);                            // update a value on the screen
        // -----------------------------------------------
        // Settings
        // -----------------------------------------------
        // for an option menu
        void loadSetting();                                         // load the current settings
        void saveSetting();                                         // apply the new settings
        void revertSetting();                                       // cancel the new settings
        // data
        std::string sv;                                             // save folder path
        bool showHUD;                                               // true = show the hud
        int32_t masterVolume;                                       // master volume value (for sound & musics, range 0-100%)
        // -----------------------------------------------
        // Modules
        // -----------------------------------------------
        #ifdef _USE_FILESYSTEM_
        FileSystem files;                                           // load and manage files to/in the memory
        #endif
        #ifdef _USE_INIFILE_
        IniFile inifile;                                            // read/write data from an ini file
        #endif
        #ifdef _USE_FONT_
        ManagerFont fonts;                                          // font loading and memory management
        #endif
        #ifdef _USE_LANG_
        ManagerLang langs;                                          // language files loading & switching
        #endif
        #ifdef _USE_MUSIC_
        ManagerMusic musics;                                        // store up to 105 music in memory (one playing at a time)
        #endif
        #ifdef _USE_SOUND_
        ManagerSound sounds;                                        // store and play up to 150 sound effects
        #endif
        #ifdef _USE_TEXTURE_
        ManagerTexture textures;                                    // load textures from memory/disk and memory management
        #endif
        #ifdef _USE_SHADER_
        ManagerShader shaders;                                    // load textures from memory/disk and memory management
        #endif
        ManagerThread threads;                                      // run task on another thread
        #ifdef _USE_MENU_
        ManagerMenu menus;                                          // load and manage menus on screen
        #endif
        #ifdef _USE_SAVE_
        Save saves;                                                 // player save data
        #endif
        #ifdef _USE_INPUT_
        Input input;                                                // player input & key binding
        #endif
        #ifdef _USE_NETWORK_
        NetworkClient client;
        NetworkServer server;
        #endif

    protected:
        // -----------------------------------------------
        // Functions
        // -----------------------------------------------
        // Engine
        // -----------------------------------------------
        inline void update();                                       // for the main loop. Main part.
        inline void render();                                       // for the main loop. Graphic part.
        inline void resize();                                       // called when the window is resized
        void create(sf::VideoMode mode, const sf::String &title,    // update the window mode / title / style / context
                    sf::Uint32 style=sf::Style::Default,
                    const sf::ContextSettings &settings=sf::ContextSettings());
        inline void garbageCollection();                            // garbage collection of the modules
        // -----------------------------------------------
        // Ini file
        // -----------------------------------------------
        inline void init_ini();                                     // initial read
        inline void load_ini();                                     // key binding loading
        inline void refresh_ini();                                  // writing new data
        // -----------------------------------------------
        // Custom
        // -----------------------------------------------
        // those functions are in game/engine.cpp
        void post_constructor();
        void pre_init();
        void post_init();
        void pre_update();
        void post_update();
        // -----------------------------------------------
        // Variables
        // -----------------------------------------------
        // Engine
        // -----------------------------------------------
        bool focus;                                                 // window focus
        enum {INIT, RUNNING, STOPPING, STOPPED} state;              // engine state
        bool game_pause;                                            // game is pauded
        sf::Event event;                                            // sfml event
        // engine tick
        int32_t game_loop_maxupdates;                               // max tick per second (default 30)
        float game_loop_updaterate;                                 // update rate in ms   (1/30 = 33ms)
        sf::Clock anUpdateClock;                                    // clock used to time the next tick
        int32_t anUpdateNext;                                       // when is the next tick
        sf::Clock tickTime;                                         // clock to get the elapsed time since last tick
        sf::Time elapsedTickTime;                                   // elapsed time since the last tick
        // others
        #ifdef _USE_ALARM_
        std::vector<Alarm*> alarms;                                 // list of alarms running
        #endif
        sf::Mutex mutex;
        // -----------------------------------------------
        // Video
        // -----------------------------------------------
        sf::RenderWindow win;                                       // game window
        sf::RenderTexture tex[2];                                   // both view textures
        sf::RenderTexture *rTex;                                    // store the pointer to current texture in use
        sf::Sprite sprite[2];                                       // sprites used to display the textures
        sf::Color color[2];                                         // color of the sprite
        sf::View view[2];                                           // camera (game & HUD)
        uint32_t sizeLimit;                                         // texture size limit supported by the graphic card
        sf::Clock elapsedTime;                                      // clock to get the elapsed time since last frame
        std::string wintitle;                                       // window name
        int8_t winmode;                                             // window mode (0 = windowed, 1 = fullscreen, 2 = borderless)
        uint32_t winfps;                                            // framerate limit
        std::vector<sf::VideoMode> modes;                           // video mode list
        int32_t currentVideoMode;                                   // current video mode
        bool system_resize;                                         // is set to true when the resize function is needed
        HUD *hud;                                                    // object managing the HUD
        // -----------------------------------------------
        // Audio
        // -----------------------------------------------
        #ifdef _USE_SOUND_
        sf::SoundBuffer* dummy_soundbuffer;                         // to force the audio thread running (to avoid it to stop/start constantly)
        #endif
        // -----------------------------------------------
        // Map
        // -----------------------------------------------
        #ifdef _USE_MAP_
        Map *map;                                                   // current map loaded
        #endif
        // -----------------------------------------------
        // Debug
        // -----------------------------------------------
        #ifdef DEBUG_BUILD
        sf::Clock debug_clock;                                      // to time when to update the debug HUD
        sf::Time debug_timer;                                       // current time
        #endif
};

extern Engine engine;

#endif // ENGINE_H
