#include "AStar.h"

#ifndef ASTAR_H
#define ASTAR_H

#include <iostream>
#include <vector>
#include <queue>
#include <set>

#endif


#include <cmath>
#include <algorithm>

AStar::AStar(const vector<vector<bool>>& grid) : grid(grid) {}

vector<Node> AStar::get_neighbours(const Node& node) {
    vector<Node> neighbours;
    int x = node.position.first;
    int y = node.position.second;
    for (pair<int, int> d : initializer_list<pair<int, int>>{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
        int nx = x + d.first, ny = y + d.second;
        if (nx >= 0 && nx < grid.size() && ny >= 0 && ny < grid[0].size() && grid[nx][ny]) {
            neighbours.push_back(Node({nx, ny}));
        }
    }
    return neighbours;
}

int AStar::find_h_cost(const Node& a, const Node& b) {
    return abs(a.position.first - b.position.first) + abs(a.position.second - b.position.second);
}

vector<pair<int, int>> AStar::reconstruct_path(Node* end_node) {
    vector<pair<int, int>> path;
    for (Node* current = end_node; current; current = current->parent) {
        path.push_back(current->position);
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<pair<int, int>> AStar::find_path(pair<int, int> start, pair<int, int> end) {
    Node* start_node = new Node(start);
    Node* end_node = new Node(end);
    open_set.push(*start_node);

    while (!open_set.empty()) {
        Node current_node = open_set.top();
        open_set.pop();

        if (current_node.position == end_node->position) {
            return reconstruct_path(&current_node);
        }

        closed_set.insert(current_node.position);

        for (Node& neighbour : get_neighbours(current_node)) {
            if (closed_set.find(neighbour.position) != closed_set.end()) continue;    //i.e., the node is already in the path

            int tentative_g_cost = current_node.g_cost + 1;
            if (tentative_g_cost < neighbour.g_cost || neighbour.g_cost == 0) {
                neighbour.g_cost = tentative_g_cost;
                neighbour.h_cost = find_h_cost(neighbour, *end_node);
                neighbour.parent = new Node(current_node);
                open_set.push(neighbour);
                //No issues if a node with the same position was already in the open_set. In that case, 
                //the one with lesser f_cost will be selected (if this positon has to be selected).
            }
        }
    }
    return {};     //If all nodes have been exhausted and the end point could not be found (No solution exists)
}

vector<pair<int, int>> find_sol(const vector<vector<bool>>& grid, pair<int, int> start, pair<int, int> end) {
    AStar a_star(grid);
    return a_star.find_path(start, end);
}