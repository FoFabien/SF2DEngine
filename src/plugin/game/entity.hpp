#ifndef ENTITY_H
#define ENTITY_H
#define _USE_ENTITY_
#ifdef _USE_ENTITY_

#include <vector>
#include <stdint.h>
#include <SFML/System/Vector2.hpp>

class Entity;
#include "map.hpp"
#include "component.hpp"

class Entity
{
    public:
        Entity();
        Entity(uint32_t id);
        Entity(uint32_t id, const std::vector<int32_t> &args);
        ~Entity();
        void init(uint32_t id);
        void init(uint32_t id, const std::vector<int32_t> &args);
        void ready();

        void update();
        void draw();
        uint32_t getID() const;
        #ifdef _USE_MAP_
        void setMap(Map* map);
        Map* getMap() const;
        #endif

        const PhysicComponent* getPhysic() const;
        BattleComponent* getBattle() const;
        GraphicComponent* getGraphic() const;
        InteractionComponent* getInteraction() const;

        sf::Vector2i pos;
        sf::Vector2i mov;
        sf::Vector2i lastPos;
        sf::Vector2i lastMov;

        // game specific (in "game_entity.cpp")
        void jump(int32_t force);
        bool isOnGround() const;
    protected:
        #ifdef _USE_MAP_
        Map* owner;
        #endif
        bool initialized;
        uint32_t ID;

        GraphicComponent* graphic;
        PhysicComponent* physic;
        AIComponent* ai;
        PlayerComponent* player;
        BattleComponent* battle;
        InteractionComponent* interaction;

};

#include "../../game/entity.hpp"

#endif
#endif // ENTITY_H
