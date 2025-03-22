#pragma once

#include <SFML/Graphics.hpp>
#include "CBS.h"
#include <utility>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <algorithm>


using namespace std;

typedef pair<int, int> Point;
typedef vector<vector<bool>> Grid;
typedef vector<pair<Point, Point>> Agents;
typedef vector<vector<Point>> Paths;


Paths solve(Grid &grid, Agents &agents);
// Convert path to direction string
string pathToDirections(const vector<Point>& path);



class GUI {
public:
    GUI();
    void run();
private:
    void handleEvents();
    void render();
    void setupFonts();



    sf::RenderWindow window;

    sf::Vector2u windowSize;
    float centerX;
    float centerY;
    
    // States
    enum State { INPUT, SETUP, ANIMATION, FINISHED, NO_PATH };
    State currentState = INPUT;
    
    // Input variables
    int m = 5, n = 5, bots = 2;
    bool inputValid = true;
    
    // Grid and agent data
    Grid grid;
    Agents agents;
    Paths paths;
    
    // Block data
    int blockWidth = 70;
    int blockSpacing = 15;

    // Warning message variables in SETUP state
    bool showWarning = false;
    float warningTimer = 0.0f;

    // Animation variables
    float cellSize = 60.f;
    float circleRadius  = cellSize/2 - 10;
    float timer = 0;
    int step = 0;
    map<Point, vector<pair<int, int>>> visitedCells;      // Maps cells to the agents that have visited them and the time of visit
    bool animationComplete = false;
    
    // Dragging variables for start/end blocks
    int draggingBlockId = -1;         // Currently dragging a start/end block or not, if not -1, then the agent number
    bool draggingStart = true;        // If currently dragging a start block or an end block

    // Dragging variables for cells
    Point lastDraggedCell = {-1, -1};
    
    // Fonts and text
    sf::Font font;

    // Clock for timing
    sf::Clock clock;

    // 10 bright colors for bots and 10 corresponding dull colors
    const std::vector<sf::Color> botColors = {
        sf::Color(255, 0, 0),    // Red
        sf::Color(0, 255, 0),    // Green
        sf::Color(0, 0, 255),    // Blue
        sf::Color(255, 255, 0),  // Yellow
        sf::Color(255, 0, 255),  // Magenta
        sf::Color(0, 255, 255),  // Cyan
        sf::Color(255, 128, 0),  // Orange
        sf::Color(128, 0, 255),  // Purple
        sf::Color(0, 128, 255),  // Light Blue
        sf::Color(128, 255, 0)   // Lime
    };
         
    const std::vector<sf::Color> dullBotColors = {
        sf::Color(200, 50, 50, 150),   // Dull Red
        sf::Color(50, 200, 50, 150),   // Dull Green
        sf::Color(50, 50, 200, 150),   // Dull Blue
        sf::Color(200, 200, 50, 150),  // Dull Yellow
        sf::Color(200, 50, 200, 150),  // Dull Magenta
        sf::Color(50, 200, 200, 150),  // Dull Cyan
        sf::Color(200, 150, 50, 150),  // Dull Orange
        sf::Color(150, 50, 200, 150),  // Dull Purple
        sf::Color(50, 150, 200, 150),  // Dull Light Blue
        sf::Color(150, 200, 50, 150)   // Dull Lime
    };









    // GUI state & animation variables (center, cell size, etc.)
    // e.g., centerX, centerY, dragging logic, timers, states, etc.

    // Declare your enums, flags, drag variables, etc. here too (e.g., State enum)
};
