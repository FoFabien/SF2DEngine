#ifndef COMPONENT_H
#define COMPONENT_H

#ifdef _USE_ENTITY_

// never include "component.hpp" !! include "entity.hpp" instead

#define ABS(x) ((x)<0 ? -(x) : (x))
#define SGN(x) ((x)<0 ? -1 : 1)
#define ENTITY_MOVE_VALUE_STEP 4

#include "../drawable/mdrawable.hpp"
class Entity;

class Component /// base class
{
    public:
        Component(Entity *e);
        virtual ~Component();
        virtual void update() = 0;
        virtual void draw() = 0;
        int32_t getType() { return cType; }

        bool canBeUpdated; /// free access to avoid function calls, it shouldn't be modified after the object creation
        bool canBeDrawn;
    protected:
        Component();
        int32_t cType;
        Entity* owner;
};

// to draw things
class GraphicComponent: public Component
{
    public:
        GraphicComponent(Entity *e);
        virtual ~GraphicComponent();
        virtual void update();
        virtual void draw();
        MDrawable& getDrawable(); /// return a reference to 'graph'
        void enable(bool b);
    protected:
        friend class Entity;
        MDrawable graph;
        bool enabled;
};

// for movement, collision and gravity
class PhysicComponent: public Component
{
    public:
        PhysicComponent(Entity *e);
        virtual ~PhysicComponent();
        virtual void update();
        virtual void draw();
        virtual bool isSolid() const; /// return true if object is solid
        virtual sf::Vector2i getPosition(); /// return collision box position
        virtual sf::Vector2i getSize(); /// return collision box size

        sf::IntRect bound; /// box position & size (read only, updated at each frame)
    protected:
        PhysicComponent();
        void movement(sf::IntRect &AABB, sf::Vector2i v); /// move the entity
        bool tryToMove(sf::IntRect &AABB, sf::Vector2i v); /// check for collision
        void updateBound(); /// update bound variable

        friend class Entity;
        sf::Vector2i off; // offset of the hitbox
        sf::Vector2i size; // size of the hitbox
        bool free; // true = can pass through everything. Overwrite ghost
        bool ghost; // true = can pass through entity but not terrain
        bool solid; // false = can be passed by others
};

// for AI
class AIComponent: public Component
{
    public:
        AIComponent(Entity *e);
        virtual ~AIComponent();
        virtual void update();
        virtual void draw();
    protected:
        AIComponent();
        friend class Entity;
};

// for player
class PlayerComponent: public Component
{
    public:
        PlayerComponent(Entity *e);
        virtual ~PlayerComponent();
        virtual void update();
        virtual void draw();
    protected:
        PlayerComponent();
        friend class Entity;
};

// for battle
class BattleComponent: public Component
{
    public:
        BattleComponent(Entity *e);
        virtual ~BattleComponent();
        virtual void update();
        virtual void draw();

        virtual void setCapability(bool b, bool type);
        virtual void setID(int8_t id, bool type);
        virtual void setHitbox(sf::IntRect box, bool type);
        virtual void setValue(int32_t v, bool type);

        virtual void setAtkType(int32_t type);
        virtual bool attack(int8_t id, const int32_t &v, const int32_t &t); // return true if did damage
        virtual bool attack(int8_t id, const sf::Vector2i& d);

        virtual bool canDefend() const;
        virtual bool canAttack() const;
        virtual bool didDamage() const;
        virtual bool damaged() const;
        virtual const sf::Vector2i& getFirstDamage();
        virtual void clearFirstDamage();

        sf::IntRect defBound, atkBound; // read only
    protected:
        friend class Entity;

        void updateBound();

        bool canDef;
        int8_t defID;
        sf::IntRect defBox;
        int32_t defValue;

        std::vector<sf::Vector2i> damages; // list of damage of current type. X = value, Y = type

        bool canAtk;
        int8_t atkID;
        sf::IntRect atkBox;
        int32_t atkValue;
        int32_t atkType;

        bool hasAttacked; // number of target damaged;
};

// for interaction
class InteractionComponent: public Component
{
    public:
        InteractionComponent(Entity *e);
        virtual ~InteractionComponent();
        virtual void update();
        virtual void draw();
        virtual bool checkInteraction(); // return true if a collision with another interaction box is found
        virtual std::vector<int32_t> popInteraction();
        virtual bool hasInteraction();
        virtual void giveParameters(std::vector<int32_t> p);
        virtual std::vector<int32_t>& getParameters();

        sf::IntRect bound; // read only
    protected:
        friend class Entity;

        void updateBound();

        sf::IntRect hitbox;
        bool canReceive;
        std::vector<int32_t> parameters;
        std::vector<std::vector<int32_t> > interacting;
};

#include "../../game/component.hpp"
#include "entity.hpp"

#endif
#endif // COMPONENT_H
