#include "GUI.h"

using namespace std;

typedef pair<int, int> Point;
typedef vector<vector<bool>> Grid;
typedef vector<pair<Point, Point>> Agents;
typedef vector<vector<Point>> Paths;

Paths solve(Grid &grid, Agents &agents) {
    Grid CBSGrid(grid);
    for(int i=0; i<CBSGrid.size(); i++){
        for(int j=0; j<CBSGrid[0].size(); j++){
            CBSGrid[i][j] = !CBSGrid[i][j];           // Since, in the CBS code, 1 represents free space and 0 represents obstacle
        }
    }
    return solve_CBS(CBSGrid, agents);
}

// Convert path to direction string
string pathToDirections(const vector<Point>& path) {
    if (path.size() <= 1) return "";
    
    string directions;
    for (int i = 1; i < path.size(); i++) {
        Point prev = path[i-1];
        Point curr = path[i];
        
        if (curr.first < prev.first) directions += "U";
        else if (curr.first > prev.first) directions += "D";
        else if (curr.second < prev.second) directions += "L";
        else if (curr.second > prev.second) directions += "R";
    }
    
    return directions;
}


GUI::GUI() : window(sf::VideoMode(2400, 1600), "Multi-Agent Path Finder"), m(5), n(5), bots(2) {
    window.setFramerateLimit(60);
    windowSize = window.getSize();
    centerX = windowSize.x / 2.0f;
    centerY = windowSize.y / 2.0f;
    setupFonts();
}

void GUI::run() {
    while (window.isOpen()) {
        handleEvents();
        render();
    }
}

void GUI::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
            
        // Handle mouse button press
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Handle button clicks based on state
                if (currentState == INPUT) {    
                    // Check if generate grid button is clicked (now centered)
                    sf::FloatRect buttonRect(centerX - 200, centerY + 240 - 50, 400, 100);
                    if (buttonRect.contains(event.mouseButton.x, event.mouseButton.y) && inputValid) {
                        // Initialize grid and agents
                        grid = Grid(m, vector<bool>(n, false));
                        agents.clear();
                        for (int i = 0; i < bots; i++) {
                            agents.push_back({{-1, -1}, {-1, -1}});
                        }
                        currentState = SETUP;
                    }
                } 
                
                else if (currentState == SETUP) {
                    // Check if any start/end block is clicked in the horizontal dragging area
                    for (int i = 0; i < bots; i++) {
                        int areaWidth = 2 * bots * (blockWidth + blockSpacing) + blockSpacing;
                        int areaHeight = blockWidth + 2 * blockSpacing;
                        float startX = centerX - areaWidth/2 + blockSpacing + i * 2 * (blockWidth + blockSpacing);  // Top left corner of S_(i+1) block
                        float startY = centerY - m*cellSize/2 - areaHeight - 30 + blockSpacing;
                        
                        sf::FloatRect startRect(startX, startY, blockWidth, blockWidth);
                        sf::FloatRect endRect(startX + blockWidth + blockSpacing, startY, blockWidth, blockWidth);
                        
                        if (startRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                            draggingBlockId = i;
                            draggingStart = true;
                        }
                        else if (endRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                            draggingBlockId = i;
                            draggingStart = false;
                        }
                    }
                    
                    // Check if generate paths button is clicked
                    sf::FloatRect buttonRect(centerX - 200, centerY + m*cellSize/2 + 40, 400, 80);
                    bool allPlaced = true;
                    for (auto &[s, e] : agents) {
                        if (s.first == -1 || e.first == -1) {
                            allPlaced = false;
                            break;
                        }
                    }
                    if (buttonRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                        if (allPlaced) {
                            // Generate paths
                            paths = solve(grid, agents);
                            if (!paths.empty()) {
                                currentState = ANIMATION;
                                step = 0;
                                timer = 0;
                                visitedCells.clear();
                                animationComplete = false;
                                
                                // Initialize visited cells with start positions
                                for (int i = 0; i < bots; i++) {
                                    visitedCells[agents[i].first].push_back({i, 0});
                                }
                            }
                            else{
                                currentState = NO_PATH;
                            }
                        } 
                        else {
                            // Show warning message
                            showWarning = true;
                            warningTimer = 0;
                        }
                    }
                }
                else if (currentState == FINISHED) {
                    // Check if any of the final buttons are clicked
                    sf::FloatRect savePathsRect(centerX - 1040, centerY + m*cellSize/2 + 40, 800, 80);
                    sf::FloatRect saveImageRect(centerX + 240, centerY + m*cellSize/2 + 40, 800, 80);
                    sf::FloatRect exitRect(centerX - 200, centerY + m*cellSize/2 + 40, 400, 80);
                    
                    if (savePathsRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                        // Save paths as text file
                        string filename = "paths.txt";
                        ofstream file(filename);
                        if (file.is_open()) {
                            for (int i = 0; i < paths.size(); i++) {
                                file << (i+1) << ": " << pathToDirections(paths[i]) << endl;
                            }
                            file.close();
                            cout << "Paths saved to " << filename << endl;
                        }
                    }
                    else if (saveImageRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                        // Save screenshot
                        sf::Texture texture;
                        texture.create(window.getSize().x, window.getSize().y);
                        texture.update(window);
                        sf::Image screenshot = texture.copyToImage();
                        sf::Image croppedScreenshot;
                        croppedScreenshot.create(n*cellSize, m*cellSize);
                        sf::IntRect captureArea(centerX - n*cellSize/2, centerY - m*cellSize/2, n*cellSize, m*cellSize);
                        croppedScreenshot.copy(screenshot, 0, 0, captureArea, true);
                        string filename = "Grid_with_bot_paths.png";
                        croppedScreenshot.saveToFile(filename);
                        cout << "Screenshot saved to " << filename << endl;
                    }
                    else if (exitRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                        // Exit application
                        window.close();
                    }
                }
                else if (currentState == NO_PATH) {
                    // Check if the exit button or back button is clicked
                    sf::FloatRect backRect(centerX - 420, centerY + 200, 400, 80);
                    sf::FloatRect exitRect(centerX + 20, centerY + 200, 400, 80);
                    if (exitRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                        // Exit application
                        window.close();
                    }
                    else if (backRect.contains(event.mouseButton.x, event.mouseButton.y)) {
                        // Go back to SETUP state
                        currentState = SETUP;
                    }
                }
            }
            else if (event.mouseButton.button == sf::Mouse::Right && currentState == SETUP) {
                // Toggle obstacles with right click
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                int gridX = (mousePos.y - (centerY - m*cellSize/2)) / (cellSize);
                int gridY = (mousePos.x - (centerX - n*cellSize/2)) / (cellSize);
                
                if (gridX >= 0 && gridX < m && gridY >= 0 && gridY < n) {
                    // Check if cell is occupied by a start/end point
                    bool occupied = false;
                    for (auto &[s, e] : agents) {
                        if ((s.first == gridX && s.second == gridY) || 
                            (e.first == gridX && e.second == gridY)) {
                            occupied = true;
                            break;
                        }
                    }
                    
                    if (!occupied) {
                        grid[gridX][gridY] = !grid[gridX][gridY];
                        lastDraggedCell = {gridX, gridY};
                    }
                }
            }
        }
        
        // Handle mouse button release
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                // Place dragged block
                if (draggingBlockId != -1 && currentState == SETUP) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int gridX = (mousePos.y - (centerY - m*cellSize/2)) / (cellSize);
                    int gridY = (mousePos.x - (centerX - n*cellSize/2)) / (cellSize);
                    
                    if (gridX >= 0 && gridX < m && gridY >= 0 && gridY < n) {
                        // Check if cell is already occupied
                        bool occupied = false;
                        for (auto &[s, e] : agents) {
                            if ((s.first == gridX && s.second == gridY) || 
                                (e.first == gridX && e.second == gridY)) {
                                occupied = true;
                                break;
                            }
                        }
                        
                        // Place block if cell is not occupied
                        if (!occupied && !grid[gridX][gridY]) {
                            if (draggingStart) {
                                agents[draggingBlockId].first = {gridX, gridY};
                            } 
                            else {
                                agents[draggingBlockId].second = {gridX, gridY};
                            }
                        }
                    }
                    
                    draggingBlockId = -1;
                }
            }
            
            lastDraggedCell = {-1, -1};
        }
        
        // Handle mouse movement
        if (event.type == sf::Event::MouseMoved) {
            // Toggle obstacles with right mouse drag
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && currentState == SETUP) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                int gridX = (mousePos.y - (centerY - m*cellSize/2)) / (cellSize);
                int gridY = (mousePos.x - (centerX - n*cellSize/2)) / (cellSize);
                
                if (gridX >= 0 && gridX < m && gridY >= 0 && gridY < n) {
                    // Check if cell is occupied by a start/end point
                    bool occupied = false;
                    for (auto &[s, e] : agents) {
                        if ((s.first == gridX && s.second == gridY) || 
                            (e.first == gridX && e.second == gridY)) {
                            occupied = true;
                            break;
                        }
                    }
                    
                    // Toggle cell if not occupied and not already toggled in this drag
                    if (!occupied && (gridX != lastDraggedCell.first || gridY != lastDraggedCell.second)) {
                        grid[gridX][gridY] = !grid[gridX][gridY];
                        lastDraggedCell = {gridX, gridY};
                    }
                }
            }
        }

        // Handle keyboard input for m, n, bots
        if (event.type == sf::Event::KeyPressed && currentState == INPUT) {
            if (event.key.code == sf::Keyboard::Up) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                    bots = min(bots + 1, min(10, 2*m*n));
                } 
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                    n = min(n + 1, 20);
                } 
                else {
                    m = min(m + 1, 20);
                }
            }
            else if (event.key.code == sf::Keyboard::Down) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
                    bots = max(bots - 1, 1);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                    n = max(n - 1, 1);
                } 
                else {
                    m = max(m - 1, 1);
                }
            }
        }
    }
}


void GUI::render() {
    // Render
    window.clear(sf::Color(200, 200, 200));
        
    // Update
    sf::Time dt = clock.restart();

    // Display warning message if user tries to generate paths before placing all blocks
    if (showWarning) {
        warningTimer += dt.asSeconds();
        
        sf::Text warningText("Please place all Start and End blocks first!", font, 28);
        warningText.setOrigin(0, warningText.getLocalBounds().height/2);
        warningText.setPosition(centerX + 210, centerY + m*cellSize/2 + 80);
        warningText.setFillColor(sf::Color::Red);
        window.draw(warningText);
        
        if (warningTimer > 4.0f) { // Display for 4 seconds
            showWarning = false;
            warningTimer = 0.0f;
        }
    }
    
    // Validate input
    inputValid = (m > 0 && m <= 20 && n > 0 && n <= 20 && bots > 0 && 2*bots <= min(20, m*n));

    // Update animation
    if (currentState == ANIMATION) {
        timer += dt.asSeconds();
        if (timer > 0.5f) {
            timer = 0;
            step++;
            
            // Update visited cells
            bool allComplete = true;
            for (int i = 0; i < bots; i++) {
                if (step < paths[i].size()) {
                    allComplete = false;
                    Point pos = paths[i][step];
                    
                    // Add current position to visited cells
                    visitedCells[pos].push_back({i, step});
                }
            }
            
            if (allComplete) {
                currentState = FINISHED;
                animationComplete = true;
                step--;
            }
        }
    }
    
    if (currentState == INPUT) {            
        // Draw title (centered)
        sf::Text titleText("Multi-Agent Path Finder", font, 60);
        titleText.setOrigin(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2);
        titleText.setPosition(centerX, centerY - 300);
        window.draw(titleText);
        
        // Draw input fields (centered)
        sf::Text rowsText("Rows (m): " + to_string(m), font, 40);
        rowsText.setOrigin(rowsText.getLocalBounds().width / 2, rowsText.getLocalBounds().height / 2);
        rowsText.setPosition(centerX, centerY - 160);
        window.draw(rowsText);
        
        // Instructions for controls
        sf::Text rowsInstr("(Use Up/Down arrow keys)", font, 32);
        rowsInstr.setOrigin(rowsInstr.getLocalBounds().width / 2, rowsInstr.getLocalBounds().height / 2);
        rowsInstr.setPosition(centerX, centerY - 110);
        rowsInstr.setFillColor(sf::Color(150, 150, 150));
        window.draw(rowsInstr);
        
        sf::Text colsText("Columns (n): " + to_string(n), font, 40);
        colsText.setOrigin(colsText.getLocalBounds().width / 2, colsText.getLocalBounds().height / 2);
        colsText.setPosition(centerX, centerY - 40);
        window.draw(colsText);
        
        sf::Text colsInstr("(Use Ctrl + Up/Down arrow keys)", font, 32);
        colsInstr.setOrigin(colsInstr.getLocalBounds().width / 2, colsInstr.getLocalBounds().height / 2);
        colsInstr.setPosition(centerX, centerY + 10);
        colsInstr.setFillColor(sf::Color(150, 150, 150));
        window.draw(colsInstr);
        
        sf::Text botsText("Agents: " + to_string(bots), font, 40);
        botsText.setOrigin(botsText.getLocalBounds().width / 2, botsText.getLocalBounds().height / 2);
        botsText.setPosition(centerX, centerY + 80);
        window.draw(botsText);
        
        sf::Text botsInstr("(Use Shift + Up/Down arrow keys)", font, 32);
        botsInstr.setOrigin(botsInstr.getLocalBounds().width / 2, botsInstr.getLocalBounds().height / 2);
        botsInstr.setPosition(centerX, centerY + 130);
        botsInstr.setFillColor(sf::Color(150, 150, 150));
        window.draw(botsInstr);
        
        // Draw generate button (centered)
        sf::RectangleShape button(sf::Vector2f(400, 100));
        button.setOrigin(button.getSize().x / 2, button.getSize().y / 2);
        button.setPosition(centerX, centerY + 240);
        button.setFillColor(inputValid ? sf::Color(100, 200, 100) : sf::Color(200, 100, 100));
        window.draw(button);
        
        sf::Text buttonText("Generate Grid", font, 40);
        buttonText.setOrigin(buttonText.getLocalBounds().width / 2, buttonText.getLocalBounds().height / 2);
        buttonText.setPosition(centerX, centerY + 232);
        window.draw(buttonText);
        
        // Add error message when 2*bots > m*n
        if (2 * bots > m * n) {
            sf::Text errorText("Error: 2 x (number of agents) must be <= number of cells (m x n)", font, 36);
            errorText.setOrigin(errorText.getLocalBounds().width / 2, errorText.getLocalBounds().height / 2);
            errorText.setPosition(centerX, centerY + 360);
            errorText.setFillColor(sf::Color::Red);
            window.draw(errorText);
        }
    }


    else if (currentState == SETUP) {
        // Draw grid - centered
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                sf::RectangleShape cell(sf::Vector2f(cellSize - 2, cellSize - 2));
                cell.setPosition(centerX - n*cellSize/2 + j*cellSize, centerY - m*cellSize/2 + i*cellSize);
                cell.setFillColor(grid[i][j] ? sf::Color::Black : sf::Color::White);
                window.draw(cell);
            }
        }
        
        // Draw draggable blocks area - horizontal above the grid
        int areaWidth = 2 * bots * (blockWidth + blockSpacing) + blockSpacing;
        int areaHeight = blockWidth + 2 * blockSpacing;
        
        sf::RectangleShape dragArea(sf::Vector2f(areaWidth, areaHeight));
        dragArea.setPosition(centerX - areaWidth/2, centerY - m*cellSize/2 - areaHeight - 30);
        dragArea.setFillColor(sf::Color(220, 220, 220));
        dragArea.setOutlineColor(sf::Color::Black);
        dragArea.setOutlineThickness(2);
        window.draw(dragArea);
        
        sf::Text dragAreaText("Drag blocks to grid", font, 32);
        dragAreaText.setOrigin(dragAreaText.getLocalBounds().width/2, dragAreaText.getLocalBounds().height);
        dragAreaText.setPosition(centerX, centerY - m*cellSize/2 - areaHeight - 40);
        window.draw(dragAreaText);
        
        // Draw start/end blocks in horizontal layout
        for (int i = 0; i < bots; i++) {
            float startX = centerX - areaWidth/2 + blockSpacing + i * 2 * (blockWidth + blockSpacing);  // Top left corner of S_(i+1) block
            float startY = centerY - m*cellSize/2 - areaHeight - 30 + blockSpacing;
            
            // Start block
            sf::RectangleShape startBlock(sf::Vector2f(blockWidth, blockWidth));
            startBlock.setPosition(startX, startY);
            startBlock.setFillColor(sf::Color(100, 200, 100));
            window.draw(startBlock);
            
            // Center the text in the block
            sf::Text startText("S" + to_string(i + 1), font, 30);
            startText.setOrigin(startText.getLocalBounds().width/2, startText.getLocalBounds().height/2);
            startText.setPosition(startX + blockWidth/2, startY + blockWidth/2 - 4);
            window.draw(startText);
            
            float endX = centerX - areaWidth/2 + blockSpacing + (i * 2 + 1) * (blockWidth + blockSpacing);  // Top left corner of E_(i+1) block
            float endY = startY;

            // End block
            sf::RectangleShape endBlock(sf::Vector2f(blockWidth, blockWidth));
            endBlock.setPosition(endX, endY);
            endBlock.setFillColor(sf::Color(200, 100, 100));
            window.draw(endBlock);
            
            // Center the text in the block
            sf::Text endText("E" + to_string(i + 1), font, 30);
            endText.setOrigin(endText.getLocalBounds().width/2, endText.getLocalBounds().height/2);
            endText.setPosition(endX + blockWidth/2, endY + blockWidth/2 - 4);
            window.draw(endText);
        }
        
        // Draw placed blocks on grid
        for (int i = 0; i < bots; i++) {
            if (agents[i].first.first != -1) {
                sf::RectangleShape startPlaced(sf::Vector2f(cellSize - 2, cellSize - 2));
                startPlaced.setPosition(centerX - n*cellSize/2 + agents[i].first.second*cellSize, 
                                       centerY - m*cellSize/2 + agents[i].first.first*cellSize);
                startPlaced.setFillColor(sf::Color(100, 200, 100));
                window.draw(startPlaced);
                
                sf::Text startTextPlaced("S" + to_string(i + 1), font, 24);
                startTextPlaced.setOrigin(startTextPlaced.getLocalBounds().width/2, startTextPlaced.getLocalBounds().height/2);
                startTextPlaced.setPosition(centerX - n*cellSize/2 + agents[i].first.second*cellSize + cellSize/2, 
                                           centerY - m*cellSize/2 + agents[i].first.first*cellSize + cellSize/2 - 4);
                window.draw(startTextPlaced);
            }
            
            if (agents[i].second.first != -1) {
                sf::RectangleShape endPlaced(sf::Vector2f(cellSize - 2, cellSize - 2));
                endPlaced.setPosition(centerX - n*cellSize/2 + agents[i].second.second*cellSize, 
                                     centerY - m*cellSize/2 + agents[i].second.first*cellSize);
                endPlaced.setFillColor(sf::Color(200, 100, 100));
                window.draw(endPlaced);
                
                sf::Text endTextPlaced("E" + to_string(i + 1), font, 24);
                endTextPlaced.setOrigin(endTextPlaced.getLocalBounds().width/2, endTextPlaced.getLocalBounds().height/2);
                endTextPlaced.setPosition(centerX - n*cellSize/2 + agents[i].second.second*cellSize + cellSize/2, 
                                         centerY - m*cellSize/2 + agents[i].second.first*cellSize + cellSize/2 - 4);
                window.draw(endTextPlaced);
            }
        }
        
        // Draw the block being dragged currently
        if (draggingBlockId != -1) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::RectangleShape draggedBlock(sf::Vector2f(blockWidth, blockWidth));
            draggedBlock.setPosition(mousePos.x - blockWidth / 2, mousePos.y - blockWidth / 2);
            draggedBlock.setFillColor(draggingStart ? sf::Color(100, 200, 100) : sf::Color(200, 100, 100));
            window.draw(draggedBlock);
            
            sf::Text draggedText(draggingStart ? "S" + to_string(draggingBlockId + 1) : 
                                              "E" + to_string(draggingBlockId + 1), font, 30);
            draggedText.setOrigin(draggedText.getLocalBounds().width/2, draggedText.getLocalBounds().height/2);
            draggedText.setPosition(mousePos.x, mousePos.y - 4);
            window.draw(draggedText);
        }
        
        // Draw generate paths button - centered
        bool allPlaced = true;
        for (auto &[s, e] : agents) {
            if (s.first == -1 || e.first == -1) {
                allPlaced = false;
                break;
            }
        }
        
        sf::RectangleShape genPathsButton(sf::Vector2f(400, 80));
        genPathsButton.setOrigin(200, 0);
        genPathsButton.setPosition(centerX, centerY + m*cellSize/2 + 40);
        genPathsButton.setFillColor(allPlaced ? sf::Color(100, 200, 100) : sf::Color(200, 100, 100));
        window.draw(genPathsButton);
        
        sf::Text genPathsText("Generate Paths", font, 40);
        genPathsText.setOrigin(genPathsText.getLocalBounds().width/2, 0);
        genPathsText.setPosition(centerX, centerY + m*cellSize/2 + 60 - 2);
        window.draw(genPathsText);
        
        // Draw instructions - centered
        sf::Text instructions("Right-click and drag on gird to toggle obstacles", font, 32);
        instructions.setOrigin(instructions.getLocalBounds().width/2, 0);
        instructions.setPosition(centerX, centerY + m*cellSize/2 + 140);
        window.draw(instructions);
    }

    else if (currentState == ANIMATION || currentState == FINISHED) {
        // Draw grid and obstacles - centered
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                sf::RectangleShape cell(sf::Vector2f(cellSize - 2, cellSize - 2));
                cell.setPosition(centerX - n*cellSize/2 + j*cellSize, centerY - m*cellSize/2 + i*cellSize);
                cell.setFillColor(grid[i][j] ? sf::Color::Black : sf::Color::White);
                window.draw(cell);
            }
        }
        
        // Draw visited cells
        for (const auto& [pos, visit] : visitedCells) {
            if (visit.empty()) continue;
            
            if (visit.size() == 1) {
                // Single agent - draw simple circle
                sf::CircleShape circle(circleRadius);
                circle.setPosition(centerX - n*cellSize/2 + pos.second*cellSize + 10, 
                                centerY - m*cellSize/2 + pos.first*cellSize + 10);
                int agentId = visit[0].first;
                if (visit[0].second==paths[agentId].size()-1 || visit[0].second == step){
                    circle.setFillColor(botColors[agentId]);
                }
                else{
                    circle.setFillColor(dullBotColors[agentId]);
                }
                window.draw(circle);
            } 
            else {
                // Multiple agents - improved pie chart with more segments
                float anglePerAgent = 360.0f / visit.size();
                float startAngle = 0.0f;
                
                for (int i = 0; i < visit.size(); i++) {
                    // Create a convex shape with more points for smoother pie segments
                    sf::ConvexShape segment;
                    segment.setPointCount(32); // More points for smoother circle
                    
                    // First point is center
                    segment.setPoint(0, sf::Vector2f(circleRadius, circleRadius)); 
                    
                    // Calculate points along arc
                    float angle1 = startAngle * 3.14159f / 180.0f;
                    float angle2 = (startAngle + anglePerAgent) * 3.14159f / 180.0f;
                    
                    // Add intermediate points for a smoother arc
                    for (int p = 0; p < 30; p++) {
                        float angle = angle1 + (angle2 - angle1) * p / 29.0f;
                        segment.setPoint(p+1, sf::Vector2f(
                            circleRadius + cos(angle) * circleRadius,
                            circleRadius + sin(angle) * circleRadius
                        ));
                    }
                    
                    // Last point connects back to center
                    segment.setPoint(31, sf::Vector2f(circleRadius, circleRadius));
                    
                    // Use predefined colors based on agent ID
                    int agentId = visit[i].first;
                    if (visit[i].second==paths[agentId].size()-1 || visit[i].second == step){
                        segment.setFillColor(botColors[agentId]);
                    }
                    else{
                        segment.setFillColor(dullBotColors[agentId]);
                    }
                    
                    segment.setPosition(centerX - n*cellSize/2 + pos.second*cellSize  + 10, 
                                    centerY - m*cellSize/2 + pos.first*cellSize + 10);
                    window.draw(segment);
                    
                    startAngle += anglePerAgent;
                }
            }
        }
        
        // Draw buttons if animation is complete
        if (currentState == FINISHED) {
            // Save paths button - centered
            sf::RectangleShape savePathsButton(sf::Vector2f(800, 80));
            savePathsButton.setPosition(centerX - 1040, centerY + m*cellSize/2 + 40);
            savePathsButton.setFillColor(sf::Color(100, 150, 200));
            window.draw(savePathsButton);
            
            sf::Text savePathsText("Download Paths as .txt file", font, 36);
            savePathsText.setOrigin(savePathsText.getLocalBounds().width/2, savePathsText.getLocalBounds().height/2);
            savePathsText.setPosition(centerX - 640, centerY + m*cellSize/2 + 80 - 4);
            window.draw(savePathsText);
            
            // Save image button - centered
            sf::RectangleShape saveImageButton(sf::Vector2f(800, 80));
            saveImageButton.setPosition(centerX + 240, centerY + m*cellSize/2 + 40);
            saveImageButton.setFillColor(sf::Color(100, 200, 150));
            window.draw(saveImageButton);
            
            sf::Text saveImageText("Download the grid above as an image", font, 36);
            saveImageText.setOrigin(saveImageText.getLocalBounds().width/2, saveImageText.getLocalBounds().height/2);
            saveImageText.setPosition(centerX + 640, centerY + m*cellSize/2 + 80 - 4);
            window.draw(saveImageText);
            
            // Exit button - centered
            sf::RectangleShape exitButton(sf::Vector2f(400, 80));
            exitButton.setPosition(centerX - 200, centerY + m*cellSize/2 + 40);
            exitButton.setFillColor(sf::Color(200, 100, 100));
            window.draw(exitButton);
            
            sf::Text exitText("Exit", font, 36);
            exitText.setOrigin(exitText.getLocalBounds().width/2, exitText.getLocalBounds().height/2);
            exitText.setPosition(centerX, centerY + m*cellSize/2 + 80 - 4);
            window.draw(exitText);
        }
    }

    else if (currentState == NO_PATH){
        // Draw warning message
        sf::Text warningText("No path found for the given configuration!", font, 36);
        warningText.setOrigin(warningText.getLocalBounds().width/2, warningText.getLocalBounds().height/2);
        warningText.setPosition(centerX, centerY);
        warningText.setFillColor(sf::Color::Red);
        window.draw(warningText);

        // Draw back button
        sf::RectangleShape backButton(sf::Vector2f(400, 80));
        backButton.setPosition(centerX - 420, centerY + 200);
        backButton.setFillColor(sf::Color(200, 200, 100));
        window.draw(backButton);

        sf::Text backText("Go back", font, 36);
        backText.setOrigin(backText.getLocalBounds().width/2, backText.getLocalBounds().height/2);
        backText.setPosition(centerX - 220, centerY + 240 - 4);
        window.draw(backText);

        // Draw exit button
        sf::RectangleShape exitButton(sf::Vector2f(400, 80));
        exitButton.setPosition(centerX + 20, centerY + 200);
        exitButton.setFillColor(sf::Color(200, 100, 100));
        window.draw(exitButton);

        sf::Text exitText("Exit", font, 36);
        exitText.setOrigin(exitText.getLocalBounds().width/2, exitText.getLocalBounds().height/2);
        exitText.setPosition(centerX + 220 , centerY + 240 - 4);
        window.draw(exitText);
    }


    window.display();
}

void GUI::setupFonts() {
    font.loadFromFile("../Font/Ubuntu-R.ttf");
}