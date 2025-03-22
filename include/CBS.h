#ifndef CBS_H
#define CBS_H

#include <vector>
#include <iostream>
#include <queue>
#include "AStar.h"

using namespace std;

struct Agent {
    pair<int, int> start;
    pair<int, int> end;
    Agent(pair<int, int> s, pair<int, int> e) : start(s), end(e) {}
};

struct Conflict {
    string conflict_type;            //"edge" or "vertex"
    Agent *agent1, *agent2;
    int time;
    pair<int, int> position;
    pair<int, int> position2;        //Used for edge conflicts only
    
    Conflict(string type, Agent *a1, Agent *a2, int t, pair<int, int> pos1, pair<int, int> pos2 = {-1, -1})
        : conflict_type(type), agent1(a1), agent2(a2), time(t), position(pos1), position2(pos2) {}
};

bool operator==(const Conflict &c1, const Conflict &c2);

struct Constraint {                //'agent' cannot be at 'position' at time='time'
    Agent *agent;
    pair<int, int> position;
    int time;
    Constraint(Agent *a, pair<int, int> pos, int t) : agent(a), position(pos), time(t) {}
};

struct Leaf {
    vector<vector<pair<int, int>>> solution;
    vector<Constraint> constraints;
    Leaf *parent;
    vector<Leaf*> children;
    vector<Agent*> agents;
    vector<pair<Conflict, vector<pair<int, int>>>> conflictState;     // To store the Conflict and state of all agents during the conflict
    
    Leaf(vector<Constraint> c, Leaf *p, vector<Agent*> a, vector<pair<Conflict, vector<pair<int, int>>>> conf);
    Conflict* find_conflict();
    int f_cost() const;
};

//Comparator function (for min-heap based on f_cost)
struct Compare {
    bool operator()(const Leaf* a, const Leaf* b);
};

class Constraint_Tree {
public:
    Leaf *root;
    vector<Agent*> agents;
    priority_queue<Leaf*, vector<Leaf*>, Compare> open;
    vector<vector<bool>> grid;
    
    Constraint_Tree();
    void update_leaf_solution(Leaf *leaf);
    vector<vector<pair<int, int>>> find_result(vector<vector<bool>> &grid, vector<Agent*> agents);
};

vector<vector<pair<int, int>>> solve_CBS(vector<vector<bool>> &grid, const vector<pair<pair<int, int>, pair<int, int>>> &agents);
bool test(vector<vector<bool>> &test_grid, vector<Agent> agents);

#endif