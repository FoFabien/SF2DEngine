#include "entity.hpp"
#include <utility>

#ifdef _USE_ENTITY_

#include "../../mlib/mlib.hpp"

Component::Component()
{
    // useless
}

Component::Component(Entity* e)
{
    canBeUpdated = false;
    canBeDrawn = false;
    owner = e;
    cType = COMP_UNDEF;
}

Component::~Component()
{

}

//#####################################################

GraphicComponent::GraphicComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = true;
    owner = e;
    cType = COMP_GRAPHIC;

    enabled = true;
}

GraphicComponent::~GraphicComponent()
{

}

void GraphicComponent::update()
{
    graph.update();
}

void GraphicComponent::draw()
{
    if(!owner || !enabled) return;
    graph.setPosition(owner->pos.x, owner->pos.y);
    graph.draw();
}

MDrawable& GraphicComponent::getDrawable()
{
    return graph;
}

void GraphicComponent::enable(bool b)
{
    enabled = b;
}

//#####################################################

PhysicComponent::PhysicComponent()
{

}

PhysicComponent::PhysicComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_PHYSIC;
    off = sf::Vector2i(0, 0);
    size = sf::Vector2i(0, 0);
    free = false;
    ghost = false;
    solid = false;
}

PhysicComponent::~PhysicComponent()
{

}

void PhysicComponent::update()
{
    if(!owner) return;
    static sf::IntRect AABB;

    #ifdef _USE_MAP_
    if(free || owner->getMap() == nullptr)
    #endif
    {
        owner->pos.x+=owner->mov.x;
        owner->pos.y+=owner->mov.y;

    }
    #ifdef _USE_MAP_
    else
    {
        AABB.left = bound.left; // left side
        AABB.top = bound.top; // top side
        AABB.width = bound.width; // right side
        AABB.height = bound.height; // bottom side

        movement(AABB, owner->mov);
        owner->pos.x = AABB.left - off.x;
        owner->pos.y = AABB.top - off.y;
    }
    #endif

    bound.left = owner->pos.x + off.x; // left side
    bound.top = owner->pos.y + off.y; // top side
    bound.width = bound.left + size.x - 1; // right side
    bound.height = bound.top + size.y - 1; // bottom side
}

void PhysicComponent::movement(sf::IntRect &AABB, sf::Vector2i v)
{
    if(v.x == 0 && v.y == 0) return;
    static sf::IntRect cpy; // to avoid construc
    if(v.x / ENTITY_MOVE_VALUE_STEP >= 1 || v.y / ENTITY_MOVE_VALUE_STEP >= 1) // recursivity, to lessen the burden for big movement
    {
        sf::Vector2i half = v/2;
        movement(AABB, half); // half the distance
		movement(AABB, v-half); // second half
		return;
    }
    bool collision = true;

    cpy.left = AABB.left;
    cpy.top = AABB.top;
    cpy.width = AABB.width;
    cpy.height = AABB.height;

    if(tryToMove(AABB, v)) collision = false; // try to move

    if(collision)
    {
        // if collision, correct the position
        for(int i = 0; i < ABS(v.x); ++i)
        {
            if (!tryToMove(AABB, sf::Vector2i(SGN(v.x),0)))
                break;
        }
        for(int i = 0; i < ABS(v.y); ++i)
        {
            if (!tryToMove(AABB, sf::Vector2i(0,SGN(v.y))))
                break;
        }
    }

    // check for special tile
    #ifdef _USE_MAP_
    sf::Vector2i correction = owner->getMap()->applySpecialTile(AABB);

    // apply correction if possible, if not cancel movement
    if((correction.x != 0 || correction.y != 0) && !tryToMove(AABB, correction)) AABB = cpy;
    #endif
}

bool PhysicComponent::tryToMove(sf::IntRect &AABB, sf::Vector2i v)
{
    #ifdef _USE_MAP_
    Map* M = owner->getMap();
    sf::IntRect moved(AABB.left+v.x, AABB.top+v.y, AABB.width+v.x, AABB.height+v.y);
    if(M->BoundIsInSolid(moved)) return false;
    if(!ghost && M->collideEntity(moved, owner)) return false;

    // refresh position
    AABB.left+=v.x;
    AABB.top+=v.y;
    AABB.width+=v.x;
    AABB.height+=v.y;
    #endif
    return true;
}

void PhysicComponent::draw()
{

}

bool PhysicComponent::isSolid() const
{
    return solid;
}

sf::Vector2i PhysicComponent::getPosition()
{
    if(!owner) return off;
    return owner->pos+off;
}

sf::Vector2i PhysicComponent::getSize()
{
    return size;
}

void PhysicComponent::updateBound()
{
    bound.left = owner->pos.x + off.x; // left side
    bound.top = owner->pos.y + off.y; // top side
    bound.width = bound.left + size.x - 1; // right side
    bound.height = bound.top + size.y - 1; // bottom side
}

//#####################################################
AIComponent::AIComponent()
{

}

AIComponent::AIComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_AI;
}

AIComponent::~AIComponent()
{

}

void AIComponent::update()
{
    // test
    owner->mov = sf::Vector2i(0, 0);
    #ifdef _USE_MAP_
    if(owner && owner->getBattle() && owner->getMap())
    {
        if(owner->getBattle()->damaged())
        {
            owner->getMap()->destroyEntity(owner);
        }
    }
    #endif
}

void AIComponent::draw()
{

}

//#####################################################
PlayerComponent::PlayerComponent()
{

}

PlayerComponent::PlayerComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_PLAYER;
}

PlayerComponent::~PlayerComponent()
{

}

void PlayerComponent::update()
{

}

void PlayerComponent::draw()
{

}

//#####################################################
BattleComponent::BattleComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_BATTLE;

    canDef = false;
    defID = -1;
    defValue = 0;

    canAtk = false;
    atkID = -1;
    atkValue = 0;
    atkType = -1;
    hasAttacked = false;
}

BattleComponent::~BattleComponent()
{

}

void BattleComponent::update()
{
    #ifdef _USE_MAP_
    if(!owner || !owner->getMap()) return;
    damages.clear();
    hasAttacked = false;

    if(canDef)
    {
        defBound.left = defBox.left + owner->pos.x;
        defBound.top = defBox.top + owner->pos.y;
        defBound.width = defBound.left + defBox.width;
        defBound.height = defBound.top + defBox.height;
    }

    if(canAtk)
    {
        atkBound.left = atkBox.left + owner->pos.x;
        atkBound.top = atkBox.top + owner->pos.y;
        atkBound.width = atkBound.left + atkBox.width;
        atkBound.height = atkBound.top + atkBox.height;
        hasAttacked = owner->getMap()->checkAttack(atkBound, atkID, sf::Vector2i(atkValue, atkType), owner);
    }
    #endif
}

void BattleComponent::draw()
{

}

void BattleComponent::setCapability(bool b, bool type)
{
    if(type) canAtk = b;
    else canDef = b;
}

void BattleComponent::setID(int8_t id, bool type)
{
    if(type) atkID = id;
    else defID = id;
}

void BattleComponent::setHitbox(sf::IntRect box, bool type)
{
    if(type) atkBox = box;
    else defBox = box;
}

void BattleComponent::setValue(int32_t v, bool type)
{
    if(type) atkValue = v;
    else defValue = v;
}

void BattleComponent::setAtkType(int32_t type)
{
    atkType = type;
}

bool BattleComponent::attack(int8_t id, const int32_t &v, const int32_t &t)
{
    if(!canDef) return false;
    if(defID == id)
    {
        damages.push_back(sf::Vector2i(v, t));
        return true;
    }
    return false;
}

bool BattleComponent::attack(int8_t id, const sf::Vector2i &d)
{
    if(!canDef) return false;
    if(defID == id)
    {
        damages.push_back(d);
        return true;
    }
    return false;
}

bool BattleComponent::canDefend() const
{
    return canDef;
}

bool BattleComponent::canAttack() const
{
    return canAtk;
}

bool BattleComponent::didDamage() const
{
    return hasAttacked;
}

bool BattleComponent::damaged() const
{
    return !damages.empty();
}

const sf::Vector2i& BattleComponent::getFirstDamage()
{
    return damages[0];
}

void BattleComponent::clearFirstDamage()
{
    damages.erase(damages.begin());
}

void BattleComponent::updateBound()
{
    if(canDef)
    {
        defBound.left = defBox.left + owner->pos.x;
        defBound.top = defBox.top + owner->pos.y;
        defBound.width = defBound.left + defBox.width;
        defBound.height = defBound.top + defBox.height;
    }
    if(canAtk)
    {
        atkBound.left = atkBox.left + owner->pos.x;
        atkBound.top = atkBox.top + owner->pos.y;
        atkBound.width = atkBound.left + atkBox.width;
        atkBound.height = atkBound.top + atkBox.height;
    }
}

//#####################################################
InteractionComponent::InteractionComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_INTERACT;
}

InteractionComponent::~InteractionComponent()
{

}

void InteractionComponent::update()
{
    bound.left = hitbox.left + owner->pos.x;
    bound.top = hitbox.top + owner->pos.y;
    bound.width = bound.left + hitbox.width;
    bound.height = bound.top + hitbox.height;
}

void InteractionComponent::draw()
{

}

void InteractionComponent::updateBound()
{
    bound.left = hitbox.left + owner->pos.x;
    bound.top = hitbox.top + owner->pos.y;
    bound.width = bound.left + hitbox.width;
    bound.height = bound.top + hitbox.height;
}

bool InteractionComponent::checkInteraction()
{
    #ifdef _USE_MAP_
    if(!owner || !owner->getMap()) return false;
    return owner->getMap()->checkInteraction(bound, interacting, parameters, owner);
    #else
    return false;
    #endif
}

std::vector<int32_t> InteractionComponent::popInteraction()
{
    std::vector<int32_t> r;
    if(interacting.empty()) return r;
    r = interacting[0];
    interacting.erase(interacting.begin());
    return r;
}

bool InteractionComponent::hasInteraction()
{
    return (!interacting.empty());
}

void InteractionComponent::giveParameters(std::vector<int32_t> p)
{
    if(!canReceive) return;
    interacting.push_back(p);
}

std::vector<int32_t>& InteractionComponent::getParameters()
{
    return parameters;
}

//#####################################################
/* TEMPLATE for copy paste
TestComponent::TestComponent(Entity *e)
{
    canBeUpdated = false;
    canBeDrawn = false;
    owner = e;
}

TestComponent::~TestComponent()
{

}

void TestComponent::update()
{

}

void TestComponent::draw()
{

}*/

#endif
