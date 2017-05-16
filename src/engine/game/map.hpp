#ifndef MAP_HPP
#define MAP_HPP
#define _USE_MAP_
#ifdef _USE_MAP_

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <vector>
#include <stdint.h>
#include "../mlib/mload.hpp"

class Map;
#include "entity.hpp"

#define MAP_VERSION 0x3530764d
#define VERTEX_MAP_SIZE_X 20
#define VERTEX_MAP_SIZE_Y 20

class Map: public MLoad, public sf::Drawable, public sf::Transformable
{
    public:
        Map();
        virtual ~Map();
        void setLoadedFlag();
        bool isLoaded();
        bool loadFailed();
        virtual void clear(); // clear all the value and containers
        sf::Vector2i getSize(); // return the map size in tile.
        sf::Vector2i getSizePixel(); // return the map size in pixel

        virtual void drawMap();
        virtual void updateMap();

        virtual void setTile(sf::Vector2i p, int16_t t, int32_t layer);
        virtual void setTileColor(sf::Vector2i p, sf::Color c, int32_t layer);
        virtual void setTileInfo(sf::Vector2i p, uint8_t t, int32_t layer);

        bool hasTileSet();
        sf::Vector2i getTileSize();
        int32_t getTileSizeX();
        int32_t getTileSizeY();
        const uint8_t *getTileInfo();

        void sortEntities();

        // entity interaction
        #ifdef _USE_ENTITY_
        bool BoundIsInSolid(sf::IntRect AABB) const;
        sf::Vector2i applySpecialTile(sf::IntRect AABB) const;
        int32_t closestGround(sf::IntRect AABB, bool useEntity, const Entity* e, int32_t limit = 0) const;
        bool collideEntity(sf::IntRect AABB, Entity* e) const;
        bool checkAttack(const sf::IntRect& AABB, int8_t id, const sf::Vector2i& damage, Entity* e) const;
        bool checkInteraction(const sf::IntRect& AABB, std::vector<std::vector<int32_t> >& container, const std::vector<int32_t> &parameters, Entity* e) const;
        void destroyEntity(Entity* ptr);
        #endif

    protected:
        virtual bool load(std::istream &f);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		inline void runScript();

        bool loaded;
        bool loadError;
        int32_t script_id;
        sf::Vector2i size; // map size in tile
        sf::Vector2i tilesize; // tile size in pixel
        uint8_t layer_count; // number of layer
        uint8_t current_layer;
        uint8_t *tiles; // tiles info
        sf::Texture* tileset;
        std::string tileset_file;
        uint8_t tw; // tileset width in tile
        uint8_t th; // tileset height in tile

		std::vector<sf::VertexArray***> m_vertices; // map vertex
        sf::Vector2i vertex_size; // vertex size (width and height)
        sf::IntRect vertex_view; // current view used by vertex
        sf::IntRect game_view; // current view

        #ifdef _USE_ENTITY_
        std::vector<Entity*> entities;
        std::vector<std::vector<Entity*> > sorted; // for display
        std::vector<Entity*> destroyed;
        #endif
};

#include "../../shared/map.hpp"

#endif
#endif // MAP_HPP
