#include "../plugin/game/map.hpp"

#ifdef _USE_MAP_
#include "../engine.hpp"

DungeonMap::DungeonMap(): Map()
{
    dummy = new sf::Texture();
    sf::Image tmp;
    uint8_t pixels[4] = {0, 0, 0, 255};
    tmp.create(1, 1, pixels);
    dummy->loadFromImage(tmp);
}

DungeonMap::~DungeonMap()
{
    clear();
    delete dummy;
}

void DungeonMap::drawMap()
{
    Map::drawMap();
    if(tileset)
    {
        current_layer = layer_count;
        engine.draw(this);
    }
}

void DungeonMap::updateMap()
{
    Map::updateMap();
}

void DungeonMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if(tileset == nullptr ||current_layer >= (uint8_t)m_vertices.size()) return;
    states.transform *= getTransform();

    if(current_layer == layer_count) states.texture = dummy;
    else states.texture = tileset;

    for(int32_t i = vertex_view.left; i < vertex_size.x && i <= vertex_view.width; ++i)
        for(int32_t j = vertex_view.top; j < vertex_size.y && j <= vertex_view.height; ++j)
            if(m_vertices[current_layer][i][j]) target.draw(*(m_vertices[current_layer][i][j]), states);
}

bool DungeonMap::load(std::istream &f)
{
    if(Map::load(f))
    {
        if(!tileset)
        {
            return true;
        }

        m_vertices.push_back(new sf::VertexArray**[vertex_size.x]);
        for(int32_t t = 0; t < vertex_size.x; ++t)
        {
            m_vertices[layer_count][t] = new sf::VertexArray*[vertex_size.y];
            for(int32_t u = 0; u < vertex_size.y; ++u)
            {
                if(t == vertex_size.x - 1 && u == vertex_size.y - 1) m_vertices[layer_count][t][u] = new sf::VertexArray(sf::Quads, ((size.x - (t * VERTEX_MAP_SIZE_X)) * (size.y - (u * VERTEX_MAP_SIZE_Y)) * 4));
                else if(t == vertex_size.x - 1) m_vertices[layer_count][t][u] = new sf::VertexArray(sf::Quads, ((size.x - (t * VERTEX_MAP_SIZE_X)) * VERTEX_MAP_SIZE_Y * 4));
                else if(u == vertex_size.y - 1) m_vertices[layer_count][t][u] = new sf::VertexArray(sf::Quads, (VERTEX_MAP_SIZE_X * (size.y - (u * VERTEX_MAP_SIZE_Y)) * 4));
                else m_vertices[layer_count][t][u] = new sf::VertexArray(sf::Quads, (VERTEX_MAP_SIZE_X * VERTEX_MAP_SIZE_Y * 4));
            }
        }

        for(int32_t x = 0; x < size.x; ++x)
        {
            for(int32_t y = 0; y < size.y; ++y)
            {
                int32_t ts;
                if(size.x - x <= (size.x % VERTEX_MAP_SIZE_X)) ts = size.x % VERTEX_MAP_SIZE_X;
                else ts = VERTEX_MAP_SIZE_X;

                sf::Vertex* quad = &((*(m_vertices[layer_count][x / VERTEX_MAP_SIZE_X][y / VERTEX_MAP_SIZE_Y]))[((x % VERTEX_MAP_SIZE_X) + ((y % VERTEX_MAP_SIZE_Y) * ts)) * 4]);

                quad[0].position = sf::Vector2f(x * tilesize.x, y * tilesize.y);
                quad[1].position = sf::Vector2f((x + 1) * tilesize.x, y * tilesize.y);
                quad[2].position = sf::Vector2f((x + 1) * tilesize.x, (y + 1) * tilesize.y);
                quad[3].position = sf::Vector2f(x * tilesize.x, (y + 1) * tilesize.y);

                quad[0].color = sf::Color(255, 255, 255, 255);
                quad[1].color = sf::Color(255, 255, 255, 255);
                quad[2].color = sf::Color(255, 255, 255, 255);
                quad[3].color = sf::Color(255, 255, 255, 255);

                quad[0].texCoords = sf::Vector2f(0, 0);
                quad[1].texCoords = sf::Vector2f(0, 0);
                quad[2].texCoords = sf::Vector2f(0, 0);
                quad[3].texCoords = sf::Vector2f(0, 0);
            }
        }
        return true;
    }
    else return false;
}

void DungeonMap::setTileColor(sf::Vector2i p, sf::Color c, int32_t layer)
{
    if(!tileset || layer >= (int32_t)m_vertices.size() || layer < 0 || p.x < 0 || p.y < 0 || p.x >= size.x || p.y >= size.y) return;

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

#endif // _USE_MAP_
