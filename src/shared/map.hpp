#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

// never include "game/map.hpp" !! include "map.hpp" instead

#ifdef _USE_MAP_
class DungeonMap: public Map
{
    public:
        DungeonMap();
        ~DungeonMap();
        virtual void drawMap();
        virtual void updateMap();
        virtual void setTileColor(sf::Vector2i p, sf::Color c, int32_t layer);
    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual bool load(std::istream &f);
        sf::Texture* dummy;
};

class RandomAdventureMap: public Map
{
    public:
        RandomAdventureMap();
        ~RandomAdventureMap();
        /*virtual void drawMap();
        virtual void updateMap();
        virtual void setTileColor(sf::Vector2i p, sf::Color c, int32_t layer);*/
    protected:
        /*virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual bool load(std::istream &f);*/
};
#endif
#endif // GAME_MAP_HPP
