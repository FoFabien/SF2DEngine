#include "astar.hpp"

// aNode class ==========================================
aNode::aNode()
{

}

aNode::~aNode()
{

}

void aNode::setPosition(sf::Vector2i p)
{
    pos = p;
}

sf::Vector2i aNode::getPosition() const
{
    return pos;
}

void aNode::addNeighbor(aNode* n)
{
    if(!n) return;
    neighbors.insert(n);
}

const std::set<aNode*>& aNode::getNeighbors() const
{
    return neighbors;
}

void aNode::setScore(int32_t s)
{
    score = s;
}

int32_t aNode::getScore() const
{
    return score;
}

// Astar class ==========================================
Astar::Astar()
{
    gState = 0;
    limited = false;
    min_score = -1;
}

Astar::~Astar()
{
    //dtor
}

bool Astar::find(aNode* start, aNode* goal)
{
    if(!startStepByStep(start, goal)) return false;
    bool result = false;
    do
    {
        result = findStepByStep();
    }while(gState != 0);
    return result;
}
bool Astar::startStepByStep(aNode* start, aNode* goal)
{
    if(gState != 0) return false;
    if(!start || !goal) return false;
    gState = 1;

    closedSet.clear();
    openSet.clear();
    cameFrom.clear();
    gScore.clear();
    fScore.clear();

    openSet.insert(start);
    gScore[start] = 0;
    fScore[start] = heuristic_cost_estimate(start, goal);
    gStart = start;
    gGoal = goal;
    gState = 2;
    return true;
}

bool Astar::findStepByStep()
{
    if(gState != 2) return false;
    gState = 1;
    if(!openSet.empty())
    {
        int32_t minScore = -1;
        aNode* current = NULL;
        for(auto &i : openSet)
        {
            if(minScore == -1 || gScore[i] < minScore)
            {
                current = i;
                minScore = gScore[i];
            }
        }
        if(current == NULL)
        {
            gState = 0;
            return false;
        }
        if(current == gGoal)
        {
            gState = 0;
            return reconstruct_path(cameFrom, current);
        }

        openSet.erase(current);
        closedSet.insert(current);

        const std::set<aNode*> neighbors = current->getNeighbors();
        for(auto &i : neighbors)
        {
            if((limited && (i->getPosition().x < search_area.left || i->getPosition().x > search_area.width || i->getPosition().y < search_area.top || i->getPosition().y > search_area.height)) || (min_score > 0 && i->getScore() >= min_score))
                continue;

            bool state = true;
            for(std::set<aNode*>::const_iterator j = closedSet.begin(); j != closedSet.end() && state; ++j)
            {
                if(i == *j)
                    state = false;
            }
            if(!state) continue;

            int32_t tentative_gScore = gScore[current] + i->getScore();
            for(std::set<aNode*>::const_iterator j = openSet.begin(); j != openSet.end() && state; ++j)
            {
                if(i == *j)
                    state = false;
            }
            if(state) openSet.insert(i);
            else if(tentative_gScore >= gScore[i])
                continue;

            cameFrom[i] = current;
            gScore[i] = tentative_gScore;
            fScore[i] = gScore[i] + heuristic_cost_estimate(i, gGoal);
        }
    }
    else
    {
        gState = 0;
        return false;
    }
    gState = 2;
    return false;
}

void Astar::stopStepByStep()
{
    gState = 0;
}

void Astar::enableSearchArea(const sf::IntRect &area)
{
    search_area = area;
    limited = true;
}

void Astar::disableSearchArea()
{
    limited = false;
}

void Astar::forbidMinScore(int32_t s)
{
    if(s > 0) min_score = s;
}

void Astar::authorizeScore()
{
    min_score = -1;
}

int32_t Astar::getState() const
{
    return gState;
}

const std::vector<aNode*>& Astar::getPath() const
{
    return total_path;
}

void Astar::swapPath(std::vector<aNode*>& v)
{
    v.swap(total_path);
    total_path.clear();
}

bool Astar::partialPath()
{
    if(gState != 0) return false;
    int32_t min = -1;
    aNode* best = NULL;
    for(auto &i : gScore)
    {
        if(i.first != gStart)
        {
            int32_t tmp = fScore[i.first] - i.second;
            if(!best || tmp < min)
            {
                min = tmp;
                best = i.first;
            }
        }
    }
    if(!best) return false;
    return reconstruct_path(cameFrom, best);
}


bool Astar::reconstruct_path(std::map<aNode*, aNode*> cameFrom, aNode* current)
{
    total_path.clear();
    total_path.push_back(current);
    while(cameFrom.find(current) != cameFrom.end())
    {
        current = cameFrom[current];
        total_path.insert(total_path.begin(), current);
    }
    return true;
}

int32_t Astar::heuristic_cost_estimate(aNode* start, aNode* goal) const
{
    if(!start || !goal) return -1;
    sf::Vector2i r(start->getPosition().x - goal->getPosition().x, start->getPosition().y - goal->getPosition().y);
    return r.x * r.x + r.y * r.y;
}
