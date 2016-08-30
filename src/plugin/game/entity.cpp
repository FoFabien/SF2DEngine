#include "entity.hpp"
#ifdef _USE_ENTITY_
#include "../../game/data.hpp"

Entity::Entity()
{
    initialized = false;
    ID = 0;
    pos.x = 0;
    pos.y = 0;
    mov.x = 0;
    mov.y = 0;
    lastPos = pos;
    lastMov = mov;
    #ifdef _USE_MAP_
    owner = nullptr;
    #endif
    graphic = nullptr;
    physic = nullptr;
    ai = nullptr;
    player = nullptr;
    battle = nullptr;
    interaction = nullptr;
}

Entity::Entity(uint32_t id): Entity()
{
    init(id);
}

Entity::Entity(uint32_t id, const std::vector<int32_t> &args): Entity()
{
    init(id, args);
}

Entity::~Entity()
{
    delete graphic;
    delete physic;
    delete ai;
    delete player;
    delete battle;
}

void Entity::init(uint32_t id)
{
    init(id, std::vector<int32_t>());
}

void Entity::init(uint32_t id, const std::vector<int32_t> &args)
{
    if(initialized) return;
    switch(id)
    {
        case 0:
            {
                graphic = new GraphicComponent(this);
                graphic->graph.loadFromData(ANIMATION, "aEntity1");
                graphic->graph.play();
                graphic->graph.setLine(1);

                physic = new JumpPhysicComponent(this);
                //physic->off = sf::Vector2i(5, 5);
                //physic->size = sf::Vector2i(22, 22);
                physic->off = sf::Vector2i(16, 10);
                physic->size = sf::Vector2i(10, 32);
                ((JumpPhysicComponent*)physic)->gravity = true;
                physic->solid = true;
                //physic->ghost = true;

                /*// test
                battle = new BattleComponent(this);
                battle->canAtk = true;
                battle->atkID = 0;
                battle->atkType = 0;
                battle->atkValue = 0;
                battle->atkBox = sf::IntRect(4, 4, 24, 24); // not for samus sprite*/

                interaction = new InteractionComponent(this);
                interaction->hitbox = sf::IntRect(0, 0, 42, 42); // samus sprite size
                interaction->canReceive = false;

                player = new PlayerComponent(this);
            }
            break;
        case 1:
            {
                graphic = new GraphicComponent(this);
                graphic->graph.loadFromData(ANIMATION, "aEntity2");
                graphic->graph.play();

                physic = new JumpPhysicComponent(this);
                physic->off = sf::Vector2i(5, 5);
                physic->size = sf::Vector2i(22, 22);
                ((JumpPhysicComponent*)physic)->gravity = true;
                physic->solid = true;

                /*battle = new BattleComponent(this);
                battle->canDef = true;
                battle->defID = 0;
                battle->defValue = 0;
                battle->defBox = sf::IntRect(4, 4, 24, 24);*/

                interaction = new InteractionComponent(this);
                interaction->hitbox = sf::IntRect(0, 0, 32, 32);
                interaction->canReceive = false;

                ai = new AIComponent(this);
            }
            break;
        case 2:
            {
                graphic = new GraphicComponent(this);
                graphic->graph.loadFromData(RECTANGLE, "");
                graphic->graph.setSize(32, 32);
                graphic->graph.setColor(sf::Color::Red);

                physic = new DungeonPhysicComponent(this);
                physic->off = sf::Vector2i(0, 0);
                physic->size = sf::Vector2i(32, 32);
                physic->solid = true;
                physic->free = true;

                player = new DungeonPlayerComponent(this);
                ((DungeonPlayerComponent*)player)->radius = 5;
            }
            break;
        case 3:
            {
                graphic = new GraphicComponent(this);
                graphic->graph.loadFromData(RECTANGLE, "");
                graphic->graph.setSize(32, 32);
                graphic->graph.setColor(sf::Color::Green);

                physic = new DungeonPhysicComponent(this);
                physic->off = sf::Vector2i(0, 0);
                physic->size = sf::Vector2i(32, 32);
                physic->solid = true;
                physic->free = true;

                ai = new DungeonAIComponent(this);
            }
            break;
        default: return;
    }
    ID = id;
    initialized = true;
}

void Entity::ready()
{
    if(physic) physic->updateBound();
    if(battle) battle->updateBound();
}

void Entity::update()
{
    if(!initialized) return;

    lastMov = mov;

    if(player && player->canBeUpdated) player->update();
    if(ai && ai->canBeUpdated) ai->update();

    lastPos = pos;

    if(physic && physic->canBeUpdated) physic->update();
    else pos+=mov;

    if(battle && battle->canBeUpdated) battle->update();
    if(interaction && interaction->canBeUpdated) interaction->update();
    if(graphic && graphic->canBeUpdated) graphic->update();
}

void Entity::draw()
{
    if(!initialized) return;
    if(graphic && graphic->canBeDrawn) graphic->draw();
    if(physic && physic->canBeDrawn) physic->draw();
    if(ai && ai->canBeDrawn) ai->draw();
    if(player && player->canBeDrawn) player->draw();
    if(battle && battle->canBeDrawn) battle->draw();
    if(interaction && interaction->canBeDrawn) interaction->draw();
}

uint32_t Entity::getID() const
{
    return ID;
}

#ifdef _USE_MAP_
void Entity::setMap(Map* map)
{
    owner = map;
}

Map* Entity::getMap() const
{
    return owner;
}
#endif

const PhysicComponent* Entity::getPhysic() const
{
    return physic;
}

BattleComponent* Entity::getBattle() const
{
    return battle;
}

GraphicComponent* Entity::getGraphic() const
{
    return graphic;
}

InteractionComponent* Entity::getInteraction() const
{
    return interaction;
}
#endif
