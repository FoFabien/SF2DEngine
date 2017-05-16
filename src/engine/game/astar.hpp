#ifndef ASTAR_HPP
#define ASTAR_HPP

#include "stdint.h"
#include <set>
#include <map>
#include <vector>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

class aNode
{
    public:
        aNode();
        ~aNode();
        void setPosition(sf::Vector2i p); // set the arbitrary position
        sf::Vector2i getPosition() const; // return the position
        void addNeighbor(aNode* n); // specify the neighbor
        const std::set<aNode*>& getNeighbors() const; // return the neighbor list
        void setScore(int32_t s); // set the score/weight of the node, higher = harder to go through the node
        int32_t getScore() const; // return the score
    protected:
        sf::Vector2i pos;
        int32_t score;
        std::set<aNode*> neighbors;
};

class Astar
{
    public:
        Astar();
        ~Astar();
        // search a path in one go
        bool find(aNode* start, aNode* goal);
        // search a path, step by step (for single threaded app)
        bool startStepByStep(aNode* start, aNode* goal); // initialize
        bool findStepByStep(); // run in a loop, return true if the search is finished (success or not)
        void stopStepByStep(); // stop the process
        // settings
        void enableSearchArea(const sf::IntRect &area); // it will avoid the specificied area. note : width and height must contains the bottom right point of the area instead of the width and height
        void disableSearchArea();
        void forbidMinScore(int32_t s); // all nodes with a score equal or superio will be ignored
        void authorizeScore();
        // others
        int32_t getState() const; // return the state of the algorithm
        const std::vector<aNode*>& getPath() const; // return the path
        void swapPath(std::vector<aNode*>& v); // swap the content of total_path to the vector
        bool partialPath(); // if the search failed
        // standalone
        bool reconstruct_path(std::map<aNode*, aNode*> cameFrom, aNode* current); // construct the path and fill total_path
        int32_t heuristic_cost_estimate(aNode* start, aNode* goal) const; // estimate the distance between two nodes (may need modifications)
    protected:
        std::vector<aNode*> total_path; // contains the resulting path
        int32_t gState; // 0=not running, 1=running, 2=idle
        // used by the algorithm
        std::set<aNode*> closedSet;
        std::set<aNode*> openSet;
        std::map<aNode*, aNode*> cameFrom;
        std::map<aNode*, int32_t> gScore;
        std::map<aNode*, int32_t> fScore;
        aNode* gStart;
        aNode* gGoal;
        bool limited;
        sf::IntRect search_area;
        int32_t min_score;
};

#endif // ASTAR_HPP
