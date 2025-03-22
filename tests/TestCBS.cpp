#define CATCH_CONFIG_MAIN
#include "catch2.hpp"
#include "CBS.h"

typedef pair<int, int> Point;

TEST_CASE("Test Case 1") {
    vector<vector<bool>> grid = {
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    vector<Agent> agents = {
        Agent({0,0}, {9,9}),
        Agent{{0,9}, {9,0}},
        Agent({0,5}, {9,4}),
        Agent({3,8}, {7,4}),
        Agent({6,2}, {8,8}),
        Agent({9,3}, {1,9})
    };
    bool result = test(grid, agents);
    REQUIRE(result==true);
}

TEST_CASE("Test Case 2") {
    vector<vector<bool>> grid = {
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    vector<Agent> agents = {
        Agent({0,0}, {9,9}),
        Agent({0,9}, {9,0})
    };
    bool result = test(grid, agents);
    REQUIRE(result==true);
}

TEST_CASE("Test Case 3") {
    vector<vector<bool>> grid = {
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    vector<Agent> agents = {
        Agent({0,0}, {9,9}),
        Agent({0,9}, {9,0}),
        Agent({0,4}, {9,5})
    };
    bool result = test(grid, agents);
    REQUIRE(result==true);
}

TEST_CASE("Test Case 4") {
    vector<vector<bool>> grid = {
        {1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 0, 1},
        {0, 1, 0, 1, 1, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 1, 1},
        {0, 1, 1, 0, 1, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 0, 1, 1, 1, 0}
    };
    vector<Agent> agents = {
        Agent({0,0}, {6,6}),
        Agent({0,5}, {5,0})
    };
    bool result = test(grid, agents);
    REQUIRE(result==true);
}

TEST_CASE("Test Case 5") {
    vector<vector<bool>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    vector<Agent> agents = {
        Agent({1,0}, {1,6}),
        Agent({1,7}, {1,0})
    };
    bool result = test(grid, agents);
    REQUIRE(result==true);
}

TEST_CASE("Test Case 6") {
    vector<vector<bool>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    vector<Agent> agents = {
        Agent({1,0}, {1,8}),
        Agent({1,8}, {1,0})
    };
    bool result = test(grid, agents);
    REQUIRE(result==false);
}

TEST_CASE("Test Case 7") {
    vector<vector<bool>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    vector<Agent> agents = {
        Agent({1,0}, {1,7}),
        Agent({1,7}, {1,0})
    };
    bool result = test(grid, agents);
    REQUIRE(result==false);
}

TEST_CASE("Test Case 8") {
    vector<vector<bool>> grid = {
        {1,1,1,1,1,1},
        {1,1,1,1,1,1},
        {1,1,1,1,1,1},
        {1,1,1,1,1,1},
        {1,1,1,1,1,1},
        {1,1,1,1,1,1}
    };
    vector<Agent> agents = {
        Agent({0,0}, {5,0}),
        Agent({1,0}, {1,4}),
        Agent({0,1}, {4,1})
    };
    bool result = test(grid, agents);
    REQUIRE(result==true);
}

TEST_CASE("Test Case 9") {
    vector<vector<bool>> grid = {
        {1,1,1,0,1,1,1,1,1},
        {1,0,0,0,0,1,1,1,1},
        {1,1,1,1,1,0,1,1,1},
        {1,1,1,1,1,0,1,1,1},
        {1,1,1,1,0,0,1,1,1},
        {1,1,1,1,0,0,1,1,1},
        {1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1}
    };
    vector<Agent> agents = {
        Agent({0,0}, {3,6}),
        Agent({2,2}, {7,3}),
        Agent({2,4}, {7,1}),
        Agent({4,0}, {1,8})
    };
    bool result = test(grid, agents);
    REQUIRE(result==true);
}

TEST_CASE("Test Case 10") {
    vector<vector<bool>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    vector<Agent> agents = {
        Agent({1,0}, {1,7}),
        Agent({1,7}, {1,0}),
        Agent({1,3}, {1,4})
    };
    bool result = test(grid, agents);
    REQUIRE(result==false);
}

TEST_CASE("Test Case 11") {
    vector<vector<bool>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    vector<Agent> agents = {
        Agent({1,0}, {1,8}),
        Agent({1,8}, {1,0}),
        Agent({1,3}, {1,4})
    };
    bool result = test(grid, agents);
    REQUIRE(result==false);
}

TEST_CASE("Test Case 12") {
    vector<vector<bool>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    vector<Agent> agents = {
        Agent({1,0}, {1,8})
    };
    bool result = test(grid, agents);
    REQUIRE(result==false);
}