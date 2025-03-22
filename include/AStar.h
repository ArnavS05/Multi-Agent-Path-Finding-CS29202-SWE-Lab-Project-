#ifndef ASTAR_H
#define ASTAR_H

#include <iostream>
#include <vector>
#include <queue>
#include <set>

using namespace std;

struct Node {
    pair<int, int> position;            //Position on the grid
    int g_cost;                         //Distance moved from the start point
    int h_cost;                         //Estimated distance to the end point, heuristic
    Node* parent;                       //The previous cell

    Node(pair<int, int> pos) : position(pos), g_cost(0), h_cost(0), parent(NULL) {}

    int f_cost() const {
        return g_cost + h_cost;
    }

    bool operator>(const Node& other) const {
        return f_cost() > other.f_cost();
    }
};

class AStar {
public:
    AStar(const vector<vector<bool>>& grid);
    //The origin is the top left corner. x is positive downwards and y is positive rightwards
    vector<pair<int, int>> find_path(pair<int, int> start, pair<int, int> end);

private:
    vector<vector<bool>> grid;
    priority_queue<Node, vector<Node>, greater<Node>> open_set;        //Min heap (according to f) of all seen nodes
    set<pair<int, int>> closed_set;                    //Set of all positions in the final path

    vector<Node> get_neighbours(const Node& node);
    int find_h_cost(const Node& a, const Node& b);
    vector<pair<int, int>> reconstruct_path(Node* end_node);
};

vector<pair<int, int>> find_sol(const vector<vector<bool>>& grid, pair<int, int> start, pair<int, int> end);

#endif