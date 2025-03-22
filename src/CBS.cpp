#include "CBS.h"
#include <algorithm>

using namespace std;

//This assumes that after reaching its end point, the agent will enter its docking station which is one of the obstacles 
//or is outside the grid or is underground.


Leaf::Leaf(vector<Constraint> c, Leaf *p, vector<Agent*> a, vector<pair<Conflict, vector<pair<int, int>>>> conf) 
    : constraints(c), parent(p), agents(a) , conflictState(conf) {}

Conflict* Leaf::find_conflict() {
    //Finding vertex conflict (only the first two conflicting agents)
    for (int i = 0; i < solution.size()-1; i++) {
        for (int j = i+1; j < solution.size(); j++) {
            for (int k = 0; k < solution[i].size(); k++) {
                if (k<solution[j].size() && solution[i][k] == solution[j][k]){    //two agents at the same place at the same time
                    pair<Conflict, vector<pair<int, int>>> GridState = make_pair(Conflict("vertex", agents[i], agents[j], k, solution[i][k]), vector<pair<int, int>>());
                    for (int l = 0; l < solution.size(); l++) {
                        if (k<solution[l].size()) GridState.second.push_back(solution[l][k]);
                        else GridState.second.push_back({-1, -1});
                    }
                    bool loop_of_conflicts = false;
                    for (int m=0; m<conflictState.size(); m++){   // i.e., detected the same conflict state of the grid again.
                        if (conflictState[m] == GridState){
                            loop_of_conflicts = true;
                            break;
                        }
                    }
                    if (loop_of_conflicts){
                        return new Conflict("loop", agents[i], agents[j], k, solution[i][k]);
                    }

                    this->conflictState.push_back(GridState);
                    return new Conflict("vertex", agents[i], agents[j], k, solution[i][k]);
                }
            }
        }
    }

    //Finding edge conflict (only the first two conflicting agents)
    for (int i = 0; i < solution.size()-1; i++) {
        for (int j = i+1; j < solution.size(); j++) {
            for (int k = 0; k < solution[i].size()-1; k++) {
                //Two agents crossing the same edge at the same time.
                if (k+1<solution[j].size() && (solution[i][k]==solution[j][k+1] && solution[i][k+1]==solution[j][k])){
                    pair<Conflict, vector<pair<int, int>>> GridState = make_pair(Conflict("edge", agents[i], agents[j], k, solution[i][k], solution[i][k+1]), vector<pair<int, int>>());
                    for (int l = 0; l < solution.size(); l++) {
                        if (k<solution[l].size()) GridState.second.push_back(solution[l][k]);
                        else GridState.second.push_back({-1, -1});
                    }
                    bool loop_of_conflicts = false;
                    for (int m=0; m<conflictState.size(); m++){   // i.e., detected the same conflict state of the grid again.
                        if (conflictState[m] == GridState){
                            loop_of_conflicts = true;
                            break;
                        }
                    }
                    if (loop_of_conflicts){
                        return new Conflict("loop", agents[i], agents[j], k, solution[i][k], solution[i][k+1]);
                    }

                    this->conflictState.push_back(GridState);
                    return new Conflict("edge", agents[i], agents[j], k, solution[i][k], solution[i][k+1]);
                }
            }
        }
    }
    return NULL;    //if no conflict found
}

bool operator==(const Conflict &c1, const Conflict &c2){
    // time is not being compared as the time of the conflict does not matter
    return c1.conflict_type == c2.conflict_type && c1.agent1 == c2.agent1 && c1.agent2 == c2.agent2 && c1.position == c2.position && c1.position2 == c2.position2;
}

bool operator==(const pair<Conflict, vector<pair<int, int>>> &p1, const pair<Conflict, vector<pair<int, int>>> &p2) {
    if (!(p1.first == p2.first)) return false;
    if (p1.second.size() != p2.second.size()) return false;
    for (size_t i = 0; i < p1.second.size(); i++) {
        if (p1.second[i] != p2.second[i]) return false;
    }
    return true;
}

int Leaf::f_cost() const {
    int cost = 0;
    for (const auto& path : solution) {
        cost += path.size();
    }
    return cost;
}

bool Compare::operator()(const Leaf* a, const Leaf* b){
    //According to Sharon et. al., 2015, tie has to be broken based on number of constraints.
    if (a->f_cost()!=b->f_cost())
        return a->f_cost() > b->f_cost();
    else           //If f_cost is same, the leaf with less constraints is preferred
        return a->constraints.size() > b->constraints.size();
}

Constraint_Tree::Constraint_Tree() {
    root = new Leaf({}, NULL, {}, vector<pair<Conflict, vector<pair<int, int>>>>());
}

void Constraint_Tree::update_leaf_solution(Leaf *leaf) {
    if (leaf->solution.empty()) {       //i.e., this leaf is the root leaf
        for (Agent* agent : agents) {
            vector<pair<int, int>> path = find_sol(grid, agent->start, agent->end);
            if (path.empty()) {
                leaf->solution.clear();
                return;                            //When no solution exists, make the solution list empty
            }
            leaf->solution.push_back(path);
        }
    }
    else {
        Constraint new_constraint = leaf->constraints.back();
        Agent* con_agent = new_constraint.agent;        //Conflicting agent
        int con_agent_num = distance(agents.begin(), find(agents.begin(), agents.end(), con_agent));  //It's index
        vector<pair<int, int>> old_con_agent_path = leaf->solution[con_agent_num];
        vector<vector<pair<int, int>>> possible_new_paths; //Possible paths for con_agent
        int con_time = new_constraint.time;

        for (int i = con_time - 1; i >= 0; i--){
            pair<int, int> pos = old_con_agent_path[i];
            for (pair<int, int> d : initializer_list<pair<int, int>>{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}) {
                int x = pos.first + d.first;
                int y = pos.second + d.second;
                if (x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size() &&
                    grid[x][y] && (make_pair(x, y) != old_con_agent_path[i + 1])){
                    
                    vector<pair<int, int>> path_part = find_sol(grid, {x, y}, con_agent->end);
                    if (!path_part.empty()) {
                        vector<pair<int, int>> new_path(old_con_agent_path.begin(), old_con_agent_path.begin() + i + 1);
                        new_path.insert(new_path.end(), path_part.begin(), path_part.end());

                        bool flag = true;
                        for (const auto& constaint : leaf->constraints){   //The new path should satisfy all the constraints of the leaf
                            if (constaint.agent == con_agent && new_path[constaint.time] == constaint.position) {
                                flag = false;
                                break;
                            }
                        }

                        if (flag) {
                            possible_new_paths.push_back(new_path);
                        }
                    }
                }
            }
        }

        if (!possible_new_paths.empty()) {
            //Choosing the shortest path from possible_new_paths
            auto min_path = min_element(possible_new_paths.begin(), possible_new_paths.end(),
                                        [](const vector<pair<int, int>>& a, const vector<pair<int, int>>& b) {
                                            return a.size() < b.size();
                                        });
            leaf->solution[con_agent_num] = *min_path;
        }
        else {
            leaf->solution.clear();
            return;
        }
    }
}

vector<vector<pair<int, int>>> Constraint_Tree::find_result(vector<vector<bool>> &grid, vector<Agent*> agents) {
    this->agents = agents;
    this->grid = grid;
    root->agents = agents;
    update_leaf_solution(root);

    if (root->solution.empty()) {        //If there is no path for at least one agent
        return {};
    }

    open.push(root);

    while (!open.empty()) {
        Leaf* current_leaf = open.top();
        open.pop();

        Conflict* conflict = current_leaf->find_conflict();

        if (!conflict) {
            return current_leaf->solution;
        }

        if(conflict->conflict_type == "loop"){    // i.e., a the same conflict with the same grid state was seen again
            delete conflict;
            continue;
        }

        if (conflict->conflict_type == "vertex"){
            for (Agent* agent : {conflict->agent1, conflict->agent2}){
                vector<pair<Conflict, vector<pair<int, int>>>> childConflictState = current_leaf->conflictState;
                Leaf* child = new Leaf(current_leaf->constraints, current_leaf, agents, childConflictState);
                child->constraints.push_back(Constraint(agent, conflict->position, conflict->time));
                current_leaf->children.push_back(child);
                child->solution = current_leaf->solution;
                update_leaf_solution(child);
                if (!child->solution.empty()) {
                    open.push(child);
                }
            }
        }
        else if (conflict->conflict_type == "edge"){
            vector<pair<Conflict, vector<pair<int, int>>>> childConflictState1 = current_leaf->conflictState;
            Leaf* child1 = new Leaf(current_leaf->constraints, current_leaf, agents, childConflictState1);
            child1->constraints.push_back(Constraint(conflict->agent1, conflict->position2, conflict->time + 1));
            current_leaf->children.push_back(child1);
            child1->solution = current_leaf->solution;
            update_leaf_solution(child1);
            if (!child1->solution.empty()) {
                open.push(child1);
            }

            vector<pair<Conflict, vector<pair<int, int>>>> childConflictState2 = current_leaf->conflictState;
            Leaf* child2 = new Leaf(current_leaf->constraints, current_leaf, agents, childConflictState2);
            child2->constraints.push_back(Constraint(conflict->agent2, conflict->position, conflict->time + 1));
            current_leaf->children.push_back(child2);
            child2->solution = current_leaf->solution;
            update_leaf_solution(child2);
            if (!child2->solution.empty()) {
                open.push(child2);
            }
        }
    }

    return {};      //If no solution could be found
}



vector<vector<pair<int, int>>> solve_CBS(vector<vector<bool>> &grid, const vector<pair<pair<int, int>, pair<int, int>>> &agents) {
    vector<Agent*> agents_;
    for (const auto &agent : agents) {
        agents_.push_back(new Agent(agent.first, agent.second));
    }

    Constraint_Tree CT;
    return CT.find_result(grid, agents_);
}

bool test(vector<vector<bool>> &test_grid, vector<Agent> agents_) {
    Constraint_Tree CT;
    vector<Agent*> agents;
    for(auto& i: agents_){
        agents.push_back(&i);
    }
    vector<vector<pair<int, int>>> res = CT.find_result(test_grid, agents);

    cout << "SAMPLE TEST\n";
    cout << "Grid: (0 represents obstacle)\n";
    cout << string(test_grid[0].size() * 2 + 2, '_') << endl;

    for (const auto &row : test_grid) {
        cout << "|";
        for (int cell : row) {
            cout << cell << " ";
        }
        cout << "|\n";
    }
    cout << string(test_grid[0].size() * 2 + 2, '-') << "\n\n";

    cout << "Agents:\n";
    for (size_t i = 0; i < agents.size(); ++i) {
        cout << i + 1 << ". (" << agents[i]->start.first << ", " << agents[i]->start.second 
             << ") -> (" << agents[i]->end.first << ", " << agents[i]->end.second << ")\n";
    }
    cout << "\n";

    bool result = true;
    if (!res.empty()) {
        cout << "Solution:\n";
        cout << "'X' represents obstacles. '.' represents path.\n";
        cout << "'s' represents start. 'g' represents goal.\n";

        for (size_t i = 0; i < agents.size(); ++i) {
            cout << "Path of agent " << i + 1 << ": ";
            for (const auto &pos : res[i]) {
                cout << "(" << pos.first << ", " << pos.second << ") ";
            }
            cout << "\n";

            set<pair<int, int>> unique_positions(res[i].begin(), res[i].end());
            if (unique_positions.size() != res[i].size()) {
                cout << "While seeing the below diagrammatic path, one should realise that\n"
                     << "the agent is passing through one (or more) point(s) more than once.\n"
                     << "Thus, one should also refer to the above list (showing the path) as well.\n";
            }

            cout << string(test_grid[0].size() * 2 + 2, '_') << endl;
            for (size_t j = 0; j < test_grid.size(); ++j) {
                cout << "|";
                for (size_t k = 0; k < test_grid[0].size(); ++k) {
                    pair<int, int> pos = {j, k};
                    if (pos == res[i][0]) {
                        cout << "s ";
                    } else if (pos == res[i].back()) {
                        cout << "g ";
                    } else if (find(res[i].begin(), res[i].end(), pos) != res[i].end()) {
                        cout << ". ";
                    } else if (test_grid[j][k] == 0) {
                        cout << "X ";
                    } else {
                        cout << "  ";
                    }
                }
                cout << "|\n";
            }
            cout << string(test_grid[0].size() * 2 + 2, '-') << "\n\n";
        }
    }
    else {
        cout << "No solution found\n";
        result = false;
    }

    cout << "\n" << string(80, '-') << "\n\n";
    return result;
}