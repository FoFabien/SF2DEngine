#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <map>
#include <stdint.h>

struct Entity
{
    uint32_t id;
    int32_t x;
    int32_t y;
};

class Map
{
    public:
        Map();
        ~Map();
        bool load(const std::string &path);
        bool save(const std::string &path);
        void drawLow();
        void drawHigh();
        void drawTileset();
        void clear();
        sf::Vector2u getSize();
        uint8_t getLayerCount();
        uint8_t getSelectedLayer();
        bool hasTileSet() {return hasTileset;}
        const sf::Texture* getTexture() {return &tileset;}
        sf::Vector2u getTileSize() {return tilesize;}
        uint8_t getTilesetWidth() {return tw;}
        uint8_t getTilesetHeight() {return tw;}
        void layerUp();
        void layerDown();
        void initEmpty();
        void toggleShowAll();
        void setSize(int32_t x, int32_t y);
        void setLayerCount(uint32_t lc);
        void deleteTileset();
        bool loadTileset(const std::string &name);
        void setTilesize(int32_t w, int32_t h);
        void setTilesetSize(int32_t tile_w, int32_t tile_h);
        bool clickTileset(sf::Vector2i pos);
        bool changeTileInfo(sf::Vector2i pos);
        bool click(sf::Vector2i pos);
        void fill(int32_t l, int32_t x, int32_t y, int16_t t, int16_t n);
        void copyEntity(sf::Vector2i pos);
        void updateTileInfo();

        int32_t script_id;
    protected:
        std::string path;
        sf::Vector2u size; // map size in tile
        sf::Vector2u tilesize; // tile size in pixel
        uint8_t layer_count; // number of layer
        uint8_t selected_layer;
        std::vector<uint8_t> t_state; // tiles info
        bool hasTileset;
        sf::Texture tileset;
        sf::Sprite sprite;
        std::string tileset_file;
        uint8_t tw; // tileset width in tile
        uint8_t th;

        std::vector<std::vector<uint16_t> > tiles;
        std::vector<std::vector<Entity> > entities;

        // others
        bool show_all;
        sf::RectangleShape hover_rect;
		sf::RectangleShape select_rect;
		sf::Texture tile_info;
		sf::Sprite tile_info_spr;

		// entity
        sf::Text etext;
        sf::ConvexShape epos;
        sf::RectangleShape einfo;
};

class BaseDial
{
    public:
        BaseDial(){};
        ~BaseDial(){};
        virtual bool click(sf::Vector2i p) = 0;
        virtual void draw() = 0;
        void setPosition(sf::Vector2i p)
        {
            pos = p;
        }
        void setPosition(int32_t x, int32_t y)
        {
            pos.x = x;
            pos.y = y;
        }
    protected:
        sf::Vector2i pos;
};

class Button: public BaseDial
{
    public:
        Button();
        Button(sf::Vector2i s, std::string sprite_file);
        virtual ~Button();
        bool click(sf::Vector2i p);
        void draw();
    protected:
        sf::Vector2i box;
        sf::Texture text;
        sf::Sprite sprite;
};

class TextDial: public BaseDial
{
    public:
        TextDial(std::string t, std::string* ptr);
        virtual ~TextDial();
        bool click(sf::Vector2i p);
        void draw();
        bool gotInput();
        void forceOk();
    protected:
        std::string title;
        std::string* str;
        sf::RectangleShape box;
        sf::Text text;
        Button *ok;
        Button *cancel;
        bool got;
        bool forced;
};

class MapEditor
{
    public:
        MapEditor();
        ~MapEditor();
        // public function
        void stop(); // change the engine state to "STOPPING"
        bool hasFocus();
        // others
        void init(); // need to be called first
        void term(); // need to be called last
        bool isRunning(); // check if the state is set to "RUNNING"
        void update(); // for the main loop. Main part.
        void render(); // for the main loop. Graphic part.
        void draw(sf::Drawable *ptr);

        sf::Vector2f getGameViewCenter();
        void moveGameView(int32_t x, int32_t y);
        void setGameViewCenter(int32_t x, int32_t y);
        sf::IntRect getViewRect();

        std::string getPath(std::string name);
        void removePath(std::string name);
        bool saveTable(std::string file);
        bool loadTable(std::string file);
        void hud();
        sf::Vector2i getMousePosition();

        // free accesss
        sf::Font font;
        int32_t current_tile;
        bool clipboard_entity;
        Entity entity;
        bool l_mouse_pressed;
        int8_t pen_mode;
        bool grid_entity;
    protected:
        void confirmInput();
        void refreshVideoMode();
        void resize();
        void create(sf::VideoMode mode, const sf::String &title, sf::Uint32 style=sf::Style::Default, const sf::ContextSettings &settings=sf::ContextSettings());

        void drawCurrentTile();
        void changeTile(char dir);

        void m_new();
        void m_load();
        void m_save();
        void m_map();
        void m_tileset();
        void m_showall();
        void m_settile(const std::string &filename, std::string w, std::string h, std::string tw, std::string th);
        void m_size(int32_t w, int32_t h);
        void m_layer(int32_t l);
        void m_entity(int32_t id);
        void m_script(int32_t id);

        // editor
        std::wstring wDir;
        Map *map;
        std::string current_map;
        std::string inputStr[10];
        TextDial* textInput;
        int8_t input_state;
        sf::RectangleShape rect; // for hud
        std::vector<Button*> buttons; // hud buttons
        std::map<std::string, std::string> trans_tab; // for file
        sf::Text text; // for hud
        char camvec[2]; // camera offset
        bool show_hud;
        uint8_t show; // determines what to show on screen
        sf::Vector2f cmap_pos;
        sf::Vector2f ctile_pos;
        sf::Sprite tilespr;
        sf::RectangleShape backspr;

        // window focus
        bool focus;
        // engine state
        enum {INIT, RUNNING, STOPPING, STOPPED} state;
        // sfml event
        sf::Event event;
        // engine tick
        int32_t game_loop_maxupdates;
        float game_loop_updaterate;
        sf::Clock anUpdateClock;
        int32_t anUpdateNext;
        // graphic
        sf::RenderWindow win;
        sf::RenderTexture tex[2];
        sf::RenderTexture *rTex;
        sf::Color color[2];
        sf::View view[2];
        sf::Clock elapsedTime;
        uint32_t sizeLimit;

        std::vector<sf::VideoMode> modes;
        int32_t currentVideoMode;
        int8_t winmode;
        uint32_t winfps;
        std::string wintitle;

        // others
        sf::Sprite sprite0, sprite1;
};

extern MapEditor engine;

#endif // MAPEDITOR_HPP
