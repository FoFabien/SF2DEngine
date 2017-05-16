#include "../engine/game/entity.hpp"

#ifdef _USE_ENTITY_
#include "../engine/engine.hpp"
#include "variable.hpp"

#include "../engine/mlib/mlib.hpp"

//#####################################################
float ground_collision_constant = 1.f;

JumpPhysicComponent::JumpPhysicComponent()
{

}

JumpPhysicComponent::JumpPhysicComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_JUMP;
    grounded = false;
    off = sf::Vector2i(0, 0);
    size = sf::Vector2i(0, 0);
    free = false;
    ghost = false;
    solid = false;
    gravity = false;
    time_air = 0;
}

JumpPhysicComponent::~JumpPhysicComponent()
{

}

void JumpPhysicComponent::update()
{
    if(!owner) return;
    static sf::IntRect AABB;

    if(gravity)
    {
        owner->mov.y += time_air;
        if(time_air < 0) time_air++;
        else time_air+=2;
        if(time_air > 20) time_air = 20;
    }

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
        int32_t movXdiff = (ABS(AABB.left - off.x - owner->pos.x) * ground_collision_constant);
        owner->pos.x = AABB.left - off.x;
        owner->pos.y = AABB.top - off.y;

        if(gravity)
        {
            if(grounded) // check state
            {
                if(movXdiff >= 0)
                {
                    movXdiff = ((movXdiff+1)*15)/10;
                    int32_t delta = owner->getMap()->closestGround(AABB, !ghost, owner, movXdiff);
                    if(delta >= 0 && delta <= movXdiff) owner->pos.y += delta;
                    else grounded = false;
                }
                else if(owner->getMap()->closestGround(AABB, !ghost, owner, 1) == -1) grounded = false;
            }
            else if(owner->getMap()->closestGround(AABB, !ghost, owner, 1) == 0) grounded = true;

            // reset time air if on ground
            if(grounded) time_air = 0;
        }
    }
    #endif

    bound.left = owner->pos.x + off.x; // left side
    bound.top = owner->pos.y + off.y; // top side
    bound.width = bound.left + size.x - 1; // right side
    bound.height = bound.top + size.y - 1; // bottom side
}

//#####################################################
DungeonPhysicComponent::DungeonPhysicComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_DPHYS;
    off = sf::Vector2i(0, 0);
    size = sf::Vector2i(0, 0);
    free = false;
    ghost = false;
    solid = false;
}

DungeonPhysicComponent::~DungeonPhysicComponent()
{

}

void DungeonPhysicComponent::update()
{
    if(!owner) return;

    owner->pos.x+=owner->mov.x;
    owner->pos.y+=owner->mov.y;

    bound.left = owner->pos.x + off.x; // left side
    bound.top = owner->pos.y + off.y; // top side
    bound.width = bound.left + size.x - 1; // right side
    bound.height = bound.top + size.y - 1; // bottom side
}

void DungeonPhysicComponent::draw()
{

}

//#####################################################
DungeonAIComponent::DungeonAIComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_DAI;

    #ifdef _USE_MAP_
    map = nullptr;
    #endif
}

DungeonAIComponent::~DungeonAIComponent()
{

}

void DungeonAIComponent::update()
{
    if(!owner) return;

    owner->mov.x = 0;
    owner->mov.y = 0;

    #ifdef _USE_MAP_
    switch(gVar[GV_GAMESTATE])
    {
        case 0: // init general
            break;
        case 1: // init enemy
            map = owner->getMap();
            if(!map) return; // to change
            tile_grid[(owner->pos.x / map->getTileSizeX()) * map->getSize().y + (owner->pos.y / map->getTileSizeY())] = 3;
            if(fov[gVar[GV_MAPID]][(owner->pos.x / map->getTileSizeX()) + (owner->pos.y / map->getTileSizeY()) * map->getSize().x] == 2) owner->getGraphic()->enable(true);
            else owner->getGraphic()->enable(false);
            break;
        case 2: // phase player start
            break;
        case 3: // phase player
            break;
        case 4: // phase player end
            break;
        case 5: // phase enemy start
            if(fov[gVar[GV_MAPID]][(owner->pos.x / map->getTileSizeX()) + (owner->pos.y / map->getTileSizeY()) * map->getSize().x] == 2) owner->getGraphic()->enable(true);
            else owner->getGraphic()->enable(false);
            break;
        case 6: // phase enemy
            break;
        case 7: // phase enemy end
            break;
        default: // others
            break;
    }
    #endif
}

void DungeonAIComponent::draw()
{

}

//#####################################################
DungeonPlayerComponent::DungeonPlayerComponent(Entity *e)
{
    canBeUpdated = true;
    canBeDrawn = false;
    owner = e;
    cType = COMP_DPLAYER;

    #ifdef _USE_MAP_
    map = nullptr;
    #endif
    key_dir = -1;
}

DungeonPlayerComponent::~DungeonPlayerComponent()
{

}

void DungeonPlayerComponent::update()
{
    if(!owner) return;

    owner->mov.x = 0;
    owner->mov.y = 0;

    #ifdef _USE_MAP_
    #ifdef _USE_INPUT_
    switch(gVar[GV_GAMESTATE])
    {
        case 0: // init general
            engine.setGameViewCenter(owner->pos.x, owner->pos.y);
            direction = 2;
            gVar[GV_GAMESTATE] = 1;
            initFOV();
            updateFOV();
            if(map)
            {
                tile_grid.clear();
                tile_grid.resize(map->getSize().x * map->getSize().y, 0);
                const uint8_t* tile_ptr = map->getTileInfo();
                if(tile_ptr)
                {
                    for(int32_t ix = 0; ix < map->getSize().x; ++ix)
                    {
                        for(int32_t iy = 0; iy < map->getSize().y; ++iy)
                        {
                            if(tile_ptr[iy+ix*map->getSize().y] > 0)
                                tile_grid[iy+ix*map->getSize().y] = 1;
                        }
                    }
                }
                tile_grid[(owner->pos.x / map->getTileSizeX()) * map->getSize().y + (owner->pos.y / map->getTileSizeY())] = 2;
            }
            break;
        case 1: // init enemy
            if(!map) return; // to change
            gVar[GV_GAMESTATE] = 2;
            engine.setScreenColor(sf::Color::White, true);
            break;
        case 2: // phase player start
            key_dir = KCOUNT;
            gVar[GV_GAMESTATE] = 3;
            gVar[GV_PLAYERX] = owner->pos.x / map->getTileSizeX();
            gVar[GV_PLAYERY] = owner->pos.y / map->getTileSizeY();
            break;
        case 3: // phase player
            {
                // pause menu test
                #ifdef _USE_MENU_
                if(engine.input.isPressedNow(KSTART))
                {
                    key_dir = KCOUNT;
                    engine.menus.addMenu(0);
                    return;
                }
                #endif

                if(key_dir == KCOUNT)
                {
                    /*****************************************
                    MOVEMENT
                    *****************************************/
                    // left key ------------------------------
                    if(engine.input.isPressed(KLEFT))
                    {
                        direction = 3;
                        key_dir = KLEFT;
                        key_time = 0;
                    }
                    // right key ------------------------------
                    else if(engine.input.isPressed(KRIGHT))
                    {
                        direction = 1;
                        key_dir = KRIGHT;
                        key_time = 0;
                    }
                    // down key -----------------------------
                    else if(engine.input.isPressed(KDOWN))
                    {
                        direction = 2;
                        key_dir = KDOWN;
                        key_time = 0;
                    }
                    // up key -----------------------------
                    else if(engine.input.isPressed(KUP))
                    {
                        direction = 0;
                        key_dir = KUP;
                        key_time = 0;
                    }
                }
                else
                {
                    if(engine.input.isReleased(key_dir))
                    {
                        key_dir = KCOUNT;
                    }

                    if(key_time < 2)
                    {
                        ++key_time;
                    }
                    else if(key_dir != KCOUNT)
                    {
                        sf::Vector2i tiletest;

                        switch(key_dir)
                        {
                            case KUP:
                                tiletest.x = owner->pos.x / map->getTileSizeX();
                                tiletest.y = (owner->pos.y / map->getTileSizeY()) - 1;
                                if(tiletest.x >= 0 && tiletest.y >= 0 && tiletest.x < map->getSize().x && tiletest.y < map->getSize().y && tile_grid[tiletest.x * map->getSize().y + tiletest.y] == 0)
                                {
                                    owner->mov.y = -map->getTileSizeY();
                                    engine.moveGameView(0, -map->getTileSizeY());
                                    tile_grid[tiletest.x * map->getSize().y + tiletest.y + 1] = 0;
                                    tile_grid[tiletest.x * map->getSize().y + tiletest.y] = 2;
                                    gVar[GV_GAMESTATE] = 4;
                                    updateFOV();
                                }
                                else key_dir = KCOUNT;
                                break;
                            case KRIGHT:
                                tiletest.x = (owner->pos.x / map->getTileSizeX()) + 1;
                                tiletest.y = owner->pos.y / map->getTileSizeY();
                                if(tiletest.x >= 0 && tiletest.y >= 0 && tiletest.x < map->getSize().x && tiletest.y < map->getSize().y && tile_grid[tiletest.x * map->getSize().y + tiletest.y] == 0)
                                {
                                    owner->mov.x = map->getTileSizeX();
                                    engine.moveGameView(map->getTileSizeX(), 0);
                                    tile_grid[(tiletest.x - 1) * map->getSize().y + tiletest.y] = 0;
                                    tile_grid[tiletest.x * map->getSize().y + tiletest.y] = 2;
                                    gVar[GV_GAMESTATE] = 4;
                                    updateFOV();
                                }
                                else key_dir = KCOUNT;
                                break;
                            case KLEFT:
                                tiletest.x = (owner->pos.x / map->getTileSizeX()) - 1;
                                tiletest.y = owner->pos.y / map->getTileSizeY();
                                if(tiletest.x >= 0 && tiletest.y >= 0 && tiletest.x < map->getSize().x && tiletest.y < map->getSize().y && tile_grid[tiletest.x * map->getSize().y + tiletest.y] == 0)
                                {
                                    owner->mov.x = -map->getTileSizeX();
                                    engine.moveGameView(-map->getTileSizeX(), 0);
                                    tile_grid[(tiletest.x + 1) * map->getSize().y + tiletest.y] = 0;
                                    tile_grid[tiletest.x * map->getSize().y + tiletest.y] = 2;
                                    gVar[GV_GAMESTATE] = 4;
                                    updateFOV();
                                }
                                else key_dir = KCOUNT;
                                break;
                            case KDOWN:
                                tiletest.x = owner->pos.x / map->getTileSizeX();
                                tiletest.y = (owner->pos.y / map->getTileSizeY()) + 1;
                                if(tiletest.x >= 0 && tiletest.y >= 0 && tiletest.x < map->getSize().x && tiletest.y < map->getSize().y && tile_grid[tiletest.x * map->getSize().y + tiletest.y] == 0)
                                {
                                    owner->mov.y = map->getTileSizeY();
                                    engine.moveGameView(0, map->getTileSizeY());
                                    tile_grid[tiletest.x * map->getSize().y + tiletest.y - 1] = 0;
                                    tile_grid[tiletest.x * map->getSize().y + tiletest.y] = 2;
                                    gVar[GV_GAMESTATE] = 4;
                                    updateFOV();
                                }
                                else key_dir = KCOUNT;
                                break;
                        }
                    }
                }
            }
            break;
        case 4: // phase player end
            gVar[GV_GAMESTATE] = 5;
            break;
        case 5: // phase enemy start
            gVar[GV_GAMESTATE] = 6;
            break;
        case 6: // phase enemy
            gVar[GV_GAMESTATE] = 7;
            break;
        case 7: // phase enemy end
            gVar[GV_GAMESTATE] = 2;
            break;
        default: // others
            break;
    }
    #endif
    #endif
}

void DungeonPlayerComponent::draw()
{

}

void DungeonPlayerComponent::initFOV()
{
    lighted.clear();
    #ifdef _USE_MAP_
    map = owner->getMap();
    if(map && map->hasTileSet() && (int32_t)fov[gVar[GV_MAPID]].size() != map->getSize().x * map->getSize().y)
        fov[gVar[GV_MAPID]].resize(map->getSize().x * map->getSize().y, 0);
    else if(!map->hasTileSet()) map = nullptr; // security
    else
    {
        sf::Vector2i p;
        for(int8_t &i: fov[gVar[GV_MAPID]])
        {
            if(i > 0)
            {
                if(i > 1) i = 1;
                p.x = i % map->getSize().x;
                p.y = i / map->getSize().x;
                map->setTileColor(p, sf::Color(255, 255, 255, 155), 2);
            }
        }
    }
    #endif
}

struct TileFOV
{
    sf::Vector2i p;
    int8_t dir;
    //123
    //804
    //765
};

void DungeonPlayerComponent::updateFOV()
{
    #ifdef _USE_MAP_
    if(!map) return;

    TileFOV tmp;
    std::vector<sf::Vector2i> newfov;

    // note
    std::vector<TileFOV> tiles;
    int32_t dist;

    tmp.p.x = (owner->pos.x + owner->mov.x) / map->getTileSizeX();
    tmp.p.y = (owner->pos.y + owner->mov.y) / map->getTileSizeY();

    tmp.dir = 0;

    sf::Vector2i origin = tmp.p;

    tiles.push_back(tmp);

    for(sf::Vector2i &i: lighted)
    {
        map->setTileColor(i, sf::Color(255, 255, 255, 155), 2);
        fov[gVar[GV_MAPID]][i.x + i.y * map->getSize().x] = 1;
    }

    while(!tiles.empty())
    {
        dist = (tiles[0].p.x - origin.x) * (tiles[0].p.x - origin.x) + (tiles[0].p.y - origin.y) * (tiles[0].p.y - origin.y);
        if(dist <= radius * radius) // if in radius
        {
            if(!owner->getMap()->BoundIsInSolid(sf::IntRect(tiles[0].p.x * owner->getMap()->getTileSizeX(), tiles[0].p.y * owner->getMap()->getTileSizeY(), tiles[0].p.x * owner->getMap()->getTileSizeX(), tiles[0].p.y * owner->getMap()->getTileSizeY())))
            { // if it's not a wall

                switch(tiles[0].dir)
                {
                    case 0:
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 1; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 2; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 3; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y; tmp.dir = 4; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 5; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 6; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 7; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y; tmp.dir = 8; tiles.push_back(tmp);
                        break;
                    case 1:
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 1; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 2; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y; tmp.dir = 8; tiles.push_back(tmp);
                        break;
                    case 2:
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 2; tiles.push_back(tmp);
                        break;
                    case 3:
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 2; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y - 1; tmp.dir = 3; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y; tmp.dir = 4; tiles.push_back(tmp);
                        break;
                    case 4:
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y; tmp.dir = 4; tiles.push_back(tmp);
                        break;
                    case 5:
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y; tmp.dir = 4; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x + 1; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 5; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 6; tiles.push_back(tmp);
                        break;
                    case 6:
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 6; tiles.push_back(tmp);
                        break;
                    case 7:
                        tmp.p.x = tiles[0].p.x; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 6; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y + 1; tmp.dir = 7; tiles.push_back(tmp);
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y; tmp.dir = 8; tiles.push_back(tmp);
                        break;
                    case 8:
                        tmp.p.x = tiles[0].p.x - 1; tmp.p.y = tiles[0].p.y; tmp.dir = 8; tiles.push_back(tmp);
                        break;
                }
            }
            newfov.push_back(tiles[0].p); // add the current tile to the list of tile in the fov
        }
        tiles.erase(tiles.begin());
    }

    for(sf::Vector2i &i: newfov)
    {
        map->setTileColor(i, sf::Color(255, 255, 255, 0), 2);
        fov[gVar[GV_MAPID]][i.x + i.y * map->getSize().x] = 2;
    }

    lighted = std::move(newfov);
    #endif
}

#endif
