#ifndef GAME_COMPONENT_HPP
#define GAME_COMPONENT_HPP

#ifdef _USE_ENTITY_

// never include "game/component.hpp" !! include "entity.hpp" instead


//cType value list
#define COMP_UNDEF      0
#define COMP_GRAPHIC    1
#define COMP_PHYSIC     2
#define COMP_AI         3
#define COMP_PLAYER     4
#define COMP_BATTLE     5
#define COMP_INTERACT   6
// custom
#define COMP_JUMP       7
#define COMP_DPHYS      8
#define COMP_DAI        9
#define COMP_DPLAYER    10

class JumpPhysicComponent: public PhysicComponent
{
    public:
        JumpPhysicComponent(Entity *e);
        virtual ~JumpPhysicComponent();
        virtual void update();
    protected:
        JumpPhysicComponent();

        friend class Entity;
        bool grounded;
        bool gravity;
        // gravity
        int32_t time_air;
};

class DungeonPhysicComponent: public PhysicComponent
{
    public:
        DungeonPhysicComponent(Entity *e);
        virtual ~DungeonPhysicComponent();
        virtual void update();
        virtual void draw();
    protected:
        friend class Entity;
};

class DungeonAIComponent: public AIComponent
{
    public:
        DungeonAIComponent(Entity *e);
        virtual ~DungeonAIComponent();
        virtual void update();
        virtual void draw();
    protected:
        #ifdef _USE_MAP_
        Map* map;
        #endif
        friend class Entity;
};

class DungeonPlayerComponent: public PlayerComponent
{
    public:
        DungeonPlayerComponent(Entity *e);
        virtual ~DungeonPlayerComponent();
        virtual void update();
        virtual void draw();
    protected:
        virtual void initFOV();
        virtual void updateFOV();
        std::vector<sf::Vector2i> lighted;
        #ifdef _USE_MAP_
        Map* map;
        #endif
        int32_t radius;
        int32_t direction;
        int8_t key_dir;
        int8_t key_time;
        friend class Entity;
};

#endif
#endif // GAME_COMPONENT_HPP
