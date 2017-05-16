#include "map.hpp"
#ifdef _USE_MAP_
#include "../engine.hpp"

#ifdef DEBUG_BUILD
#include "../mlib/mlib.hpp"
#endif

Map::Map()
{
    script_id = -1;
    size = sf::Vector2i(0, 0);
    tilesize = sf::Vector2i(0, 0);
    tiles = nullptr;
    tileset = nullptr;
    tileset_file = "";
    tw = 0;
    th = 0;
    layer_count = 0;

	vertex_size = sf::Vector2i(0, 0);
    vertex_view = sf::IntRect(0, 0, 0, 0);
    game_view = sf::IntRect(0, 0, 0, 0);

    loaded = false;
    loadError = false;
}

Map::~Map()
{
    clear();
}

void Map::setLoadedFlag()
{
    loaded = true;
}

bool Map::isLoaded()
{
    return loaded;
}

bool Map::loadFailed()
{
    return loadError;
}

void Map::clear()
{
	for(size_t i = 0; i < m_vertices.size(); ++i)
    {
        for(int32_t j = 0; j < vertex_size.x; ++j)
        {
            for(int32_t k = 0; k < vertex_size.y; ++k)
                delete m_vertices[i][j][k];
            delete [] m_vertices[i][j];
        }
        delete [] m_vertices[i];
    }
    vertex_size = sf::Vector2i(0, 0);

    script_id = -1;
	size = sf::Vector2i(0, 0);
    tilesize = sf::Vector2i(0, 0);
    tiles = nullptr;
    #ifdef _USE_TEXTURE_
    engine.textures.trash(tileset_file);
    #endif
    tileset = nullptr;
    tileset_file = "";
    tw = 0;
    th = 0;
    layer_count = 0;

    if(tiles != nullptr)
    {
        delete [] tiles;
        tiles = nullptr;
    }

    #ifdef _USE_ENTITY_
    for(Entity* &i : entities) delete i;
    entities.clear();
    sorted.clear();
    destroyed.clear();
    #endif

    loaded = false;
    loadError = false;
}

bool Map::load(std::istream &f)
{
    uint32_t tmp;
    char c;
    clear();
    f.read((char*)&tmp, 4);
    if(tmp != MAP_VERSION)
    {
        loadError = true;
        return false;
    }
    f.read((char*)&script_id, 4);
    f.read((char*)&size.x, 4);
    f.read((char*)&size.y, 4);
    f.read((char*)&tilesize.x, 4);
    f.read((char*)&tilesize.y, 4);
    f.read((char*)&layer_count, 1);

    tmp = 0;
    f.read((char*)&tmp, 1);
    if(tmp != 0)
    {
        do
        {
            if(!f.good())
            {
                loadError = true;
                return false;
            }
            f.read(&c, 1);
            if(c != 0) tileset_file += c;
        }while(c != 0);

        f.read((char*)&tw, 1);
        f.read((char*)&th, 1);
        if(tw == 0 || th == 0)
        {
            loadError = true;
            return false;
        }

        #ifdef _USE_TEXTURE_
        tileset = engine.textures.use(tileset_file);
        #endif
        if(!tileset || size.x <= 0 || size.y <= 0 || tilesize.x <= 1 || tilesize.y <= 1)
        {
            loadError = true;
            return false;
        }

        tiles = new uint8_t[tw*th]; // reading tiles info
        f.read((char*)tiles, tw*th);

        uint8_t* terrain = new uint8_t[size.x*size.y];

        vertex_size.x = (size.x / VERTEX_MAP_SIZE_X) + 1;
        vertex_size.y = (size.y / VERTEX_MAP_SIZE_Y) + 1;

        for(uint32_t i = 0; i < layer_count; ++i)
        {
            m_vertices.push_back(new sf::VertexArray**[vertex_size.x]);
            for(int32_t t = 0; t < vertex_size.x; ++t)
            {
                m_vertices[i][t] = new sf::VertexArray*[vertex_size.y];
                for(int32_t u = 0; u < vertex_size.y; ++u)
                {
                    if(t == vertex_size.x - 1 && u == vertex_size.y - 1) m_vertices[i][t][u] = new sf::VertexArray(sf::Quads, ((size.x - (t * VERTEX_MAP_SIZE_X)) * (size.y - (u * VERTEX_MAP_SIZE_Y)) * 4));
                    else if(t == vertex_size.x - 1) m_vertices[i][t][u] = new sf::VertexArray(sf::Quads, ((size.x - (t * VERTEX_MAP_SIZE_X)) * VERTEX_MAP_SIZE_Y * 4));
                    else if(u == vertex_size.y - 1) m_vertices[i][t][u] = new sf::VertexArray(sf::Quads, (VERTEX_MAP_SIZE_X * (size.y - (u * VERTEX_MAP_SIZE_Y)) * 4));
                    else m_vertices[i][t][u] = new sf::VertexArray(sf::Quads, (VERTEX_MAP_SIZE_X * VERTEX_MAP_SIZE_Y * 4));
                }
            }

            uint16_t tile_id;
            for(int32_t x = 0; x < size.x; ++x)
            {
                for(int32_t y = 0; y < size.y; ++y)
                {
                    if(!f.good())
                    {
                        loadError = true;
                        return false;
                    }
                    f.read((char*)&tile_id, 2);

                    // terrain process
                    if(tile_id >= 0 && tile_id < tw*th)
                    {
                        if(i == 0) terrain[y+x*size.y] = tiles[tile_id];
                        else if(terrain[y+x*size.y] == 0) terrain[y+x*size.y] = tiles[tile_id];
                        /*else
                        {
                            switch(terrain[y+x*size.y]) // to change later
                            {
                                case 0: terrain[y+x*size.y] = tiles[tile_id]; break;
                                default: break;
                            }
                        }*/
                    }
                    else if(i == 0) terrain[y+x*size.y] = 0;

                    // back to vertex array
                    int32_t tu = tile_id % tw;
                    int32_t tv = tile_id / tw;
                    int32_t ts;
                    if(size.x - x <= (size.x % VERTEX_MAP_SIZE_X)) ts = size.x % VERTEX_MAP_SIZE_X;
                    else ts = VERTEX_MAP_SIZE_X;

                    sf::Vertex* quad = &((*(m_vertices[i][x / VERTEX_MAP_SIZE_X][y / VERTEX_MAP_SIZE_Y]))[((x % VERTEX_MAP_SIZE_X) + ((y % VERTEX_MAP_SIZE_Y) * ts)) * 4]);

                    quad[0].position = sf::Vector2f(x * tilesize.x, y * tilesize.y);
                    quad[1].position = sf::Vector2f((x + 1) * tilesize.x, y * tilesize.y);
                    quad[2].position = sf::Vector2f((x + 1) * tilesize.x, (y + 1) * tilesize.y);
                    quad[3].position = sf::Vector2f(x * tilesize.x, (y + 1) * tilesize.y);

                    quad[0].texCoords = sf::Vector2f(tu * tilesize.x, tv * tilesize.y);
                    quad[1].texCoords = sf::Vector2f((tu + 1) * tilesize.x, tv * tilesize.y);
                    quad[2].texCoords = sf::Vector2f((tu + 1) * tilesize.x, (tv + 1) * tilesize.y);
                    quad[3].texCoords = sf::Vector2f(tu * tilesize.x, (tv + 1) * tilesize.y);
                }
            }
        }

        delete [] tiles; // useless now
        tiles = terrain;

        for(size_t a = 0; a < m_vertices.size(); ++a)
            for(int32_t b = 0; b < vertex_size.x; ++b)
                for(int32_t c = 0; c < vertex_size.y; ++c)
                {
                    bool ok = false;
                    for(uint32_t d = 0; !ok && d < m_vertices[a][b][c]->getVertexCount() && !ok; d+=4)
                    {
                        if((*(m_vertices[a][b][c]))[d].texCoords != sf::Vector2f(0, 0)) ok = true;
                    }
                    if(!ok)
                    {
                        delete m_vertices[a][b][c];
                        m_vertices[a][b][c] = nullptr;
                    }
                }
    }

    tmp = 0;
    f.read((char*)&tmp, 2);

    #ifdef _USE_ENTITY_
    for(uint16_t i = 0; i < layer_count; ++i)
         sorted.push_back(std::vector<Entity*>());
    #endif

    for(uint32_t i = 0; i < tmp; i++)
    {
        if(!f.good())
        {
            loadError = true;
            return false;
        }

        uint32_t id;
        int32_t x;
        int32_t y;
        uint8_t l;

        f.read((char*)&id, 4);
        f.read((char*)&x, 4);
        f.read((char*)&y, 4);
        f.read((char*)&l, 1);
        #ifdef _USE_ENTITY_
        if(l >= 0 && l < (int32_t)layer_count)
        {
            Entity* ptr = new Entity(id);
            ptr->pos = sf::Vector2i(x, y);
            ptr->setMap(this);
            ptr->ready();
            sorted[l].push_back(ptr);
            entities.push_back(ptr);
        }
        #endif
    }
    return true;
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if(tileset == nullptr ||current_layer >= (uint8_t)m_vertices.size()) return;
    states.transform *= getTransform();
    states.texture = tileset;

    for(int32_t i = vertex_view.left; i < vertex_size.x && i <= vertex_view.width; ++i)
        for(int32_t j = vertex_view.top; j < vertex_size.y && j <= vertex_view.height; ++j)
            if(m_vertices[current_layer][i][j]) target.draw(*(m_vertices[current_layer][i][j]), states);
}

sf::Vector2i Map::getSize()
{
    return size;
}

sf::Vector2i Map::getSizePixel()
{
    if(!tileset) return size;
    return sf::Vector2i(size.x*tilesize.x, size.y*tilesize.y);
}

void Map::drawMap()
{
    for(current_layer = 0; current_layer < layer_count; ++current_layer)
    {
        if(current_layer >= 0 && current_layer < layer_count)
        {
            if(tileset) engine.draw(this);
            #ifdef _USE_ENTITY_
            for(Entity* &i : sorted[current_layer]) i->draw();
            #endif
        }
    }
}

void Map::updateMap()
{
    if(tileset)
    {
        game_view = engine.getGameBounds();
        vertex_view.left = (game_view.left / tilesize.x) / VERTEX_MAP_SIZE_X;
        vertex_view.top = (game_view.top / tilesize.y) / VERTEX_MAP_SIZE_Y;
        vertex_view.width = ((game_view.left + game_view.width) / tilesize.x) / VERTEX_MAP_SIZE_X;
        vertex_view.height = ((game_view.top + game_view.height) / tilesize.y) / VERTEX_MAP_SIZE_Y;
        if(vertex_view.left < 0) vertex_view.left = 0;
        if(vertex_view.top < 0) vertex_view.top = 0;
    }

    runScript();

    #ifdef _USE_ENTITY_
    for(Entity* &i : entities) i->update();
    sortEntities();

    // clean
    for(size_t i = 0; i < destroyed.size(); ++i)
    {
        for(size_t j = 0; j < entities.size(); ++j)
        {
            if(entities[j] == destroyed[i])
            {
                entities.erase(entities.begin()+j);
                --j;
            }
        }

        for(size_t l = 0; l < sorted.size(); l++)
        {
            for(size_t j = 0; j < sorted[l].size(); ++j)
            {
                if(sorted[l][j] == destroyed[i])
                {
                    sorted[l].erase(sorted[l].begin()+j);
                    --j;
                }
            }
        }
        delete destroyed[i];
    }
    destroyed.clear();
    #endif
}

void Map::setTile(sf::Vector2i p, int16_t t, int32_t layer)
{
    if(!tileset || layer >= layer_count || layer < 0 || p.x < 0 || p.y < 0 || p.x >= size.x || p.y >= size.y || t >= tw * th || t < 0) return;

    int32_t a = p.x / VERTEX_MAP_SIZE_X;
    int32_t b = p.y / VERTEX_MAP_SIZE_Y;

    sf::Vector2i vp;

    if(a == vertex_size.x - 1) vp.x = (size.x - (a * VERTEX_MAP_SIZE_X));
    else vp.x = VERTEX_MAP_SIZE_X;

    if(m_vertices[layer][a][b] == nullptr)
    {
        if(b == vertex_size.y - 1) vp.y = (size.y - (b * VERTEX_MAP_SIZE_Y));
        else vp.y = VERTEX_MAP_SIZE_Y;

        m_vertices[layer][a][b] = new sf::VertexArray(sf::Quads, vp.x * vp.y * 4);

        for(int32_t i = 0; i < vp.x * vp.y; ++i)
        {
            sf::Vertex* quad = &((*(m_vertices[layer][a][b]))[i * 4]);

            quad[0].position = sf::Vector2f(p.x * tilesize.x, p.y * tilesize.y);
            quad[1].position = sf::Vector2f((p.x + 1) * tilesize.x, p.y * tilesize.y);
            quad[2].position = sf::Vector2f((p.x + 1) * tilesize.x, (p.y + 1) * tilesize.y);
            quad[3].position = sf::Vector2f(p.x * tilesize.x, (p.y + 1) * tilesize.y);

            quad[0].texCoords = sf::Vector2f(0, 0);
            quad[1].texCoords = sf::Vector2f(tilesize.x, 0);
            quad[2].texCoords = sf::Vector2f(0, tilesize.y);
            quad[3].texCoords = sf::Vector2f(tilesize.x, tilesize.y);
        }
    }

    sf::Vertex* quad = &((*(m_vertices[layer][a][b]))[((p.x % VERTEX_MAP_SIZE_X) + (p.y % VERTEX_MAP_SIZE_Y) * vp.x) * 4]);

    quad[0].texCoords = sf::Vector2f((t % tw) * tilesize.x, (t / th) * tilesize.y);
    quad[1].texCoords = sf::Vector2f(((t % tw) + 1) * tilesize.x, (t / th) * tilesize.y);
    quad[2].texCoords = sf::Vector2f(((t % tw) + 1) * tilesize.x, ((t / th) + 1) * tilesize.y);
    quad[3].texCoords = sf::Vector2f((t % tw) * tilesize.x, ((t / th) + 1) * tilesize.y);
}

void Map::setTileColor(sf::Vector2i p, sf::Color c, int32_t layer)
{
    if(!tileset || layer >= layer_count || layer < 0 || p.x < 0 || p.y < 0 || p.x >= size.x || p.y >= size.y) return;

    int32_t a = p.x / VERTEX_MAP_SIZE_X;
    int32_t b = p.y / VERTEX_MAP_SIZE_Y;

    sf::Vector2i vp;

    if(a == vertex_size.x - 1) vp.x = (size.x - (a * VERTEX_MAP_SIZE_X));
    else vp.x = VERTEX_MAP_SIZE_X;

    if(m_vertices[layer][a][b] == nullptr)
    {
        if(b == vertex_size.y - 1) vp.y = (size.y - (b * VERTEX_MAP_SIZE_Y));
        else vp.y = VERTEX_MAP_SIZE_Y;

        m_vertices[layer][a][b] = new sf::VertexArray(sf::Quads, vp.x * vp.y * 4);

        for(int32_t i = 0; i < vp.x * vp.y; ++i)
        {
            sf::Vertex* quad = &((*(m_vertices[layer][a][b]))[i * 4]);

            quad[0].position = sf::Vector2f(p.x * tilesize.x, p.y * tilesize.y);
            quad[1].position = sf::Vector2f((p.x + 1) * tilesize.x, p.y * tilesize.y);
            quad[2].position = sf::Vector2f((p.x + 1) * tilesize.x, (p.y + 1) * tilesize.y);
            quad[3].position = sf::Vector2f(p.x * tilesize.x, (p.y + 1) * tilesize.y);

            quad[0].texCoords = sf::Vector2f(0, 0);
            quad[1].texCoords = sf::Vector2f(tilesize.x, 0);
            quad[2].texCoords = sf::Vector2f(0, tilesize.y);
            quad[3].texCoords = sf::Vector2f(tilesize.x, tilesize.y);
        }
    }

    sf::Vertex* quad = &((*(m_vertices[layer][a][b]))[((p.x % VERTEX_MAP_SIZE_X) + (p.y % VERTEX_MAP_SIZE_Y) * vp.x) * 4]);

    quad[0].color = c;
    quad[1].color = c;
    quad[2].color = c;
    quad[3].color = c;
}

void Map::setTileInfo(sf::Vector2i p, uint8_t t, int32_t layer)
{
    if(!tileset || layer >= layer_count || layer < 0 || p.x < 0 || p.y < 0 || p.x >= size.x || p.y >= size.y) return;
    tiles[p.y+p.x*size.y] = t;
}

bool Map::hasTileSet()
{
    return (tileset != nullptr);
}

sf::Vector2i Map::getTileSize()
{
    return tilesize;
}

int32_t Map::getTileSizeX()
{
    return tilesize.x;
}

int32_t Map::getTileSizeY()
{
    return tilesize.y;
}

const uint8_t *Map::getTileInfo()
{
    return tiles;
}

void Map::sortEntities()
{
    Entity* e1;
    Entity* e2;
    for(uint32_t l = 0; l < layer_count; ++l)
    {
        std::vector<Entity*>& vref = sorted[l];
        if(vref.size() > 1)
        {
            for(size_t i = 0; i < vref.size(); ++i)
            {
                e1 = vref[i];
                if(e1->getGraphic())
                {
                    for(size_t j = i + 1; j < vref.size(); ++j)
                    {
                        e2 = vref[j];
                        if(e2->getGraphic())
                        {
                            if(e1->pos.x > e2->pos.x && e1->pos.y > e2->pos.y)
                            {
                                vref[i] = e2;
                                vref[j] = e1;
                            }
                        }

                    }
                }
            }
        }
    }
}

bool Map::BoundIsInSolid(sf::IntRect AABB) const
{
    if(!tileset) return false;
    sf::IntRect range;
    range.left = AABB.left / tilesize.x;
    range.top = AABB.top / tilesize.y;
    range.width = AABB.width / tilesize.x;
    range.height = AABB.height / tilesize.y;

    if(range.left >= (int32_t)size.x || range.top >= (int32_t)size.y || range.width < 0 || range.height < 0) return false;

    for(int32_t i = range.left; i <= range.width; ++i)
    {
        for(int32_t j = range.top; j <= range.height; ++j)
        {
            if(i >= 0 && j >= 0 && i < (int32_t)size.x && j < (int32_t)size.y)
            {
                if(tiles[j+i*size.y] == 1) return true;
            }
        }
    }
    return false;
}

sf::Vector2i Map::applySpecialTile(sf::IntRect AABB) const
{
    if(!tileset) return sf::Vector2i(0, 0); // nothing
    sf::Vector2i p[5];
    // point ID per position
    //   1
    // 4 0 2
    //   3

    p[0].x = AABB.left+(AABB.width-AABB.left)/2;
    p[0].y = AABB.top+(AABB.height-AABB.top)/2;

    p[1].x = p[0].x;
    p[1].y = AABB.top;

    p[2].x = AABB.width;
    p[2].y = p[0].y;

    p[3].x = p[0].x;
    p[3].y = AABB.height;

    p[4].x = AABB.left;
    p[4].y = p[0].y;

    sf::Vector2i result(0, 0);
    sf::Vector2i t, pt;
    std::vector<sf::Vector2i> tile_corrected;
    for(int32_t i = 1; i < 5; ++i)
    {
        t.x = p[i].x / tilesize.x; // tile
        t.y = p[i].y / tilesize.y;

        bool found = false;
        for(size_t j = 0; j < tile_corrected.size() && !found; ++j)
        {
            if(tile_corrected[j] == t) found = true;
        }
        if(!found) tile_corrected.push_back(t);
    }

    for(size_t j = 0; j < tile_corrected.size(); ++j)
    {
        switch(tiles[tile_corrected[j].y+tile_corrected[j].x*size.y])
        {
            case 2: // top
                {
                    t.x = p[3].x / tilesize.x; // tile
                    t.y = p[3].y / tilesize.y;
                    if(t.x != tile_corrected[j].x || t.y != tile_corrected[j].y) break;
                    pt.y = p[3].y % tilesize.y;
                    result.y -= pt.y + 1;
                    break;
                }
            case 3: // right
                {
                    t.x = p[4].x / tilesize.x; // tile
                    t.y = p[4].y / tilesize.y;
                    if(t.x != tile_corrected[j].x || t.y != tile_corrected[j].y) break;
                    pt.x = p[4].x % tilesize.x;
                    result.x += tilesize.x - pt.x;
                    break;
                }
            case 4: // bot
                {
                    t.x = p[1].x / tilesize.x; // tile
                    t.y = p[1].y / tilesize.y;
                    if(t.x != tile_corrected[j].x || t.y != tile_corrected[j].y) break;
                    pt.y = p[1].y % tilesize.y;
                    result.y += tilesize.y - pt.y;
                    break;
                }
            case 5: // left
                {
                    t.x = p[2].x / tilesize.x; // tile
                    t.y = p[2].y / tilesize.y;
                    if(t.x != tile_corrected[j].x || t.y != tile_corrected[j].y) break;
                    pt.x = p[2].x % tilesize.x;
                    result.x -= pt.x + 1;
                    break;
                }
            case 6: // /B
                {
                    t.x = p[3].x / tilesize.x; // tile
                    t.y = p[3].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[3].x % tilesize.x; // position IN tile
                    pt.y = (p[3].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 100 - (pt.x*100/(tilesize.x-1));
                    if(pt.y >= prcent*(tilesize.y-1)/100) result.y -= pt.y - (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 7: // \B
                {
                    t.x = p[3].x / tilesize.x; // tile
                    t.y = p[3].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[3].x % tilesize.x; // position IN tile
                    pt.y = (p[3].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = (pt.x*100/(tilesize.x-1));
                    if(pt.y >= prcent*(tilesize.y-1)/100) result.y -= pt.y - (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 8: // \T
                {
                    t.x = p[1].x / tilesize.x; // tile
                    t.y = p[1].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[1].x % tilesize.x; // position IN tile
                    pt.y = (p[1].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = (pt.x*100/(tilesize.x-1));
                    if(pt.y <= prcent*(tilesize.y-1)/100) result.y += - pt.y + (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 9: // /T
                {
                    t.x = p[1].x / tilesize.x; // tile
                    t.y = p[1].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[1].x % tilesize.x; // position IN tile
                    pt.y = (p[1].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 100 - (pt.x*100/(tilesize.x-1));
                    if(pt.y <= prcent*(tilesize.y-1)/100) result.y -= pt.y - (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 10: // /B1
                {
                    t.x = p[3].x / tilesize.x; // tile
                    t.y = p[3].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[3].x % tilesize.x; // position IN tile
                    pt.y = (p[3].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 100 - (pt.x*50/(tilesize.x-1));
                    if(pt.y >= prcent*(tilesize.y-1)/100) result.y -= pt.y - (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 11: // /B2
                {
                    t.x = p[3].x / tilesize.x; // tile
                    t.y = p[3].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[3].x % tilesize.x; // position IN tile
                    pt.y = (p[3].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 50 - (pt.x*50/(tilesize.x-1));
                    if(pt.y >= prcent*(tilesize.y-1)/100) result.y -= pt.y - (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 12: // \B2
                {
                    t.x = p[3].x / tilesize.x; // tile
                    t.y = p[3].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[3].x % tilesize.x; // position IN tile
                    pt.y = (p[3].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = (pt.x*50/(tilesize.x-1));
                    if(pt.y >= prcent*(tilesize.y-1)/100) result.y -= pt.y - (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 13: // \B1
                {
                    t.x = p[3].x / tilesize.x; // tile
                    t.y = p[3].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[3].x % tilesize.x; // position IN tile
                    pt.y = (p[3].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 50 + (pt.x*50/(tilesize.x-1));
                    if(pt.y >= prcent*(tilesize.y-1)/100) result.y -= pt.y - (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }

            case 14: // \T1
                {
                    t.x = p[1].x / tilesize.x; // tile
                    t.y = p[1].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[1].x % tilesize.x; // position IN tile
                    pt.y = (p[1].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = (pt.x*50/(tilesize.x-1));
                    if(pt.y <= prcent*(tilesize.y-1)/100) result.y += - pt.y + (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 15: // \T2
                {
                    t.x = p[1].x / tilesize.x; // tile
                    t.y = p[1].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[1].x % tilesize.x; // position IN tile
                    pt.y = (p[1].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 50 + (pt.x*50/(tilesize.x-1));
                    if(pt.y <= prcent*(tilesize.y-1)/100) result.y += - pt.y + (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 16: // /T2
                {
                    t.x = p[1].x / tilesize.x; // tile
                    t.y = p[1].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[1].x % tilesize.x; // position IN tile
                    pt.y = (p[1].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 100 - (pt.x*50/(tilesize.x-1));
                    if(pt.y <= prcent*(tilesize.y-1)/100) result.y += - pt.y + (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
            case 17: // /T1
                {
                    t.x = p[1].x / tilesize.x; // tile
                    t.y = p[1].y / tilesize.y;
                    if(t.x != tile_corrected[j].x) break;
                    pt.x = p[1].x % tilesize.x; // position IN tile
                    pt.y = (p[1].y - tile_corrected[j].y*tilesize.y);
                    int32_t prcent = 50 - (pt.x*50/(tilesize.x-1));
                    if(pt.y <= prcent*(tilesize.y-1)/100) result.y += - pt.y + (prcent*(tilesize.y-1)/100) + 1;
                    break;
                }
        }
    }

    return result;
}

int32_t Map::closestGround(sf::IntRect AABB, bool useEntity, const Entity* e, int32_t limit) const
{
    if(!tileset) return 0;
    int32_t stop;

    sf::Vector2i bot; // bottom left corner
    bot.x = AABB.left;
    bot.y = AABB.height;
    int32_t botW = AABB.width-AABB.left; // AABB width
    int32_t mX = (bot.x + botW / 2) / tilesize.x;
    int32_t pX = (bot.x + botW / 2) % tilesize.x;

    if(limit > 0) stop = limit;
    else stop = (tilesize.y * size.y) - bot.y - 1;

    if(e == nullptr) useEntity = false;
    Entity* ptr;
    const PhysicComponent* phc;
    sf::IntRect target_AABB;

    sf::Vector2i pt, t, prev;

    int32_t d;
    for(d = 1; d <= stop; ++d)
    {
        prev.x = -1;
        prev.y = -1;

        for(size_t i = 0; useEntity && i < entities.size(); ++i)
        {
            ptr = entities[i];
            if(ptr == nullptr || ptr == e) continue;
            phc = ptr->getPhysic();
            if(phc == nullptr || !phc->isSolid()) continue;

            if((bot.x <= phc->bound.width) && ((bot.y + d) <= phc->bound.height)
                && (bot.x + botW >= phc->bound.left) && ((bot.y + d) >= phc->bound.top))
                return d - 1;
        }


        pt.y = (bot.y + d) % tilesize.y;
        t.y = (bot.y + d) / tilesize.y;
        for(int32_t i = 0; i <= botW; ++i)
        {
            pt.x = (bot.x + i) % tilesize.x;
            t.x = (bot.x + i) / tilesize.x;

            if(t.x < 0 || t.y < 0 || t.x >= size.x || t.y >= size.y || t == prev) continue;
            prev = t;

            switch(tiles[t.y+t.x*size.y])
            {
                case 0: break;
                case 1: return d-1;
                case 2: // top
                    {
                        if(mX != t.x) break;
                        return d - 1;
                    }
                case 3: // right
                    break;
                case 4: // bot
                    break;
                case 5: // left
                    break;
                case 6: // /B
                    {
                        if(mX != t.x) break;
                        int32_t prcent = 100 - (pX*100/(tilesize.x-1));
                        if(pt.y >= prcent*(tilesize.y-1)/100) return d - 1;
                        break;
                    }
                case 7: // \B
                    {
                        if(mX != t.x) break;
                        int32_t prcent = (pX*100/(tilesize.x-1));
                        if(pt.y >= prcent*(tilesize.y-1)/100) return d - 1;
                        break;
                    }
                case 8: // \T
                    break;
                case 9: // /T
                    break;
                case 10: // /B1
                    {
                        if(mX != t.x) break;
                        int32_t prcent = 100 - (pX*50/(tilesize.x-1));
                        if(pt.y >= prcent*(tilesize.y-1)/100) return d - 1;
                        break;
                    }
                case 11: // /B2
                    {
                        if(mX != t.x) break;
                        int32_t prcent = 50 - (pX*50/(tilesize.x-1));
                        if(pt.y >= prcent*(tilesize.y-1)/100) return d - 1;
                        break;
                    }
                case 12: // \B2
                    {
                        if(mX != t.x) break;
                        int32_t prcent = (pX*50/(tilesize.x-1));
                        if(pt.y >= prcent*(tilesize.y-1)/100) return d - 1;
                        break;
                    }
                case 13: // \B1
                    {
                        if(mX != t.x) break;
                        int32_t prcent = 50 + (pX*50/(tilesize.x-1));
                        if(pt.y >= prcent*(tilesize.y-1)/100) return d - 1;
                        break;
                    }
                case 14: // \T1
                    break;
                case 15: // \T2
                    break;
                case 16: // /T2
                    break;
                case 17: // /T1
                    break;
            }
        }
    }
    return -1;
}

bool Map::collideEntity(sf::IntRect AABB, Entity* e) const
{
    if(e == nullptr) return false;
    const PhysicComponent* phc;
    for(auto &i : entities)
    {
        if(i == nullptr || i == e) continue;
        phc = i->getPhysic();
        if(phc == nullptr || !phc->isSolid()) continue;

        if((AABB.left <= phc->bound.width) && (AABB.top <= phc->bound.height)
           && (AABB.width >= phc->bound.left) && (AABB.height >= phc->bound.top))
           return true;
    }
    return false;
}

bool Map::checkAttack(const sf::IntRect& AABB, int8_t id, const sf::Vector2i& damage, Entity* e) const
{
    if(e == nullptr) return false;
    BattleComponent* bc;
    bool result = false;

    for(auto &i : entities)
    {
        if(i == nullptr || i == e) continue;
        bc = i->getBattle();
        if(bc == nullptr || !bc->canDefend()) continue;

        if((AABB.left <= bc->defBound.width) && (AABB.top <= bc->defBound.height)
           && (AABB.width >= bc->defBound.left) && (AABB.height >= bc->defBound.top))
        {
            if(bc->attack(id, damage)) result = true;
        }
    }
    return result;
}

bool Map::checkInteraction(const sf::IntRect& AABB, std::vector<std::vector<int32_t> >& container, const std::vector<int32_t> &parameters, Entity* e) const
{
    if(e == nullptr) return false;
    InteractionComponent* ic;
    bool result = false;

    for(auto &i : entities)
    {
        if(i == nullptr || i == e) continue;
        ic = i->getInteraction();
        if(ic == nullptr) continue;

        if((AABB.left <= ic->bound.width) && (AABB.top <= ic->bound.height)
           && (AABB.width >= ic->bound.left) && (AABB.height >= ic->bound.top))
        {
            container.push_back(ic->getParameters());
            ic->giveParameters(parameters);
            result = true;
        }
    }
    return result;
}

#ifdef _USE_ENTITY_
void Map::destroyEntity(Entity* ptr)
{
    if(std::find(destroyed.begin(), destroyed.end(), ptr) == destroyed.end()) destroyed.push_back(ptr);
}
#endif

void Map::runScript()
{
    switch(script_id)
    {
        #ifdef _USE_MENU_
        case 0: engine.menus.addMenu(2); script_id = -1; break;
        #endif
        default: return;
    }
}
#endif
