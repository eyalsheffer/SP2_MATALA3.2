#include "GameGui.hpp"
#include <iostream>
//#include <string>
#include <cmath>

GameGui::GameGui(int playerCount) 
    : window(sf::VideoMode(1200, 800), "Coup - Main Game")
    , font()
    , fontLoaded(false)
    , numPlayers(playerCount)
    , game(&Game::instance())
{
    std::cout << "GameGui constructor started with " << playerCount << " players" << std::endl;
    
    // Try to load font
    if (!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf") &&
        !font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cout << "Warning: Could not load font. Using default font." << std::endl;
        fontLoaded = false;
    } else {
        fontLoaded = true;
    }
    
    std::cout << "About to initialize colors" << std::endl;
    initializeColors();
    
    std::cout << "About to initialize players" << std::endl;
    initializePlayers();
    
    std::cout << "About to initialize UI" << std::endl;
    initializeUI();
    
    std::cout << "About to initialize action buttons" << std::endl;
    initializeActionButtons();
    
    std::cout << "Constructor completed successfully" << std::endl;
    updateInfoPanel("Game Started! Player 1's turn.");
}

void GameGui::initializeColors() {
    backgroundColor = sf::Color(25, 35, 45);        // Dark blue
    panelColor = sf::Color(45, 55, 65);             // Lighter blue-gray
    activePlayerColor = sf::Color(100, 149, 237);   // Cornflower blue
    inactivePlayerColor = sf::Color(70, 80, 90);    // Gray
    buttonColor = sf::Color(80, 100, 120);          // Blue-gray
    buttonHoverColor = sf::Color(135, 206, 235);    // Sky blue
    textColor = sf::Color::White;
}

void GameGui::initializePlayers() {
     // Clear existing players first
    std::vector<Player*>& players = game->get_players();
    for(Player* p : players) {
        delete p;
    }
    players.clear();

    playersGui.resize(numPlayers);
    players.reserve(numPlayers);
    for (int i = 0; i < numPlayers; i++) {
        Player* newPlayer = new Player("Player " + std::to_string(i + 1));
        newPlayer->set_coins(2);  
        newPlayer->set_isActive(true);
        players.push_back(newPlayer);
    }
    
    setupPlayerPositions();
}

void GameGui::setupPlayerPositions() {
    // Arrange players in a circle around the screen
    float centerX = 600.0f;
    float centerY = 400.0f;
    float radius = 280.0f;
    
    for (int i = 0; i < numPlayers; i++) {
        float angle = (2.0f * M_PI * i) / numPlayers - M_PI / 2; // Start from top
        
        playersGui[i].position.x = centerX + radius * cos(angle) - 80; // Offset for card width
        playersGui[i].position.y = centerY + radius * sin(angle) - 60; // Offset for card height
        
        // Player card background
        playersGui[i].playerCard.setSize(sf::Vector2f(160, 120));
        playersGui[i].playerCard.setPosition(playersGui[i].position);
        playersGui[i].playerCard.setFillColor(inactivePlayerColor);
        
        // Player name
        playersGui[i].nameText.setString(game->get_players()[i]->get_name());
        if (fontLoaded) playersGui[i].nameText.setFont(font);
        playersGui[i].nameText.setCharacterSize(16);
        playersGui[i].nameText.setFillColor(textColor);
        playersGui[i].nameText.setStyle(sf::Text::Bold);
        playersGui[i].nameText.setPosition(playersGui[i].position.x + 10, playersGui[i].position.y + 10);
        
        // Coins display
        playersGui[i].coinsText.setString("Coins: " + std::to_string(game->get_players()[i]->get_coins()));
        if (fontLoaded) playersGui[i].coinsText.setFont(font);
        playersGui[i].coinsText.setCharacterSize(14);
        playersGui[i].coinsText.setFillColor(sf::Color(255, 215, 0)); // Gold
        playersGui[i].coinsText.setPosition(playersGui[i].position.x + 10, playersGui[i].position.y + 40);
        
        // Influence display
        // players[i].influenceText.setString("Cards: " + std::to_string(players[i].influence));
        // if (fontLoaded) players[i].influenceText.setFont(font);
        // players[i].influenceText.setCharacterSize(14);
        // players[i].influenceText.setFillColor(sf::Color(255, 100, 100)); // Light red
        // players[i].influenceText.setPosition(players[i].position.x + 10, players[i].position.y + 65);
    }
    
    updatePlayerDisplay();
}

void GameGui::initializeUI() {
    // Title
    titleText.setString("COUP");
    if (fontLoaded) titleText.setFont(font);
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color(255, 215, 0)); // Gold
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(20, 20);
    
    // Current player indicator
    currentPlayerText.setString("Current Player: Player 1");
    if (fontLoaded) currentPlayerText.setFont(font);
    currentPlayerText.setCharacterSize(20);
    currentPlayerText.setFillColor(textColor);
    currentPlayerText.setPosition(20, 70);
    
    // Phase indicator
    phaseText.setString("Phase: Action Selection");
    if (fontLoaded) phaseText.setFont(font);
    phaseText.setCharacterSize(16);
    phaseText.setFillColor(textColor);
    phaseText.setPosition(20, 100);
    
    // Instruction text
    instructionText.setString("Choose an action:");
    if (fontLoaded) instructionText.setFont(font);
    instructionText.setCharacterSize(16);
    instructionText.setFillColor(textColor);
    instructionText.setPosition(20, 130);
    
    // Info panel
    infoPanel.setSize(sf::Vector2f(350, 200));
    infoPanel.setPosition(20, 580);
    infoPanel.setFillColor(panelColor);
    
    infoPanelText.setString("Game Log:\nGame Started!");
    if (fontLoaded) infoPanelText.setFont(font);
    infoPanelText.setCharacterSize(12);
    infoPanelText.setFillColor(textColor);
    infoPanelText.setPosition(30, 590);
}

void GameGui::initializeActionButtons() {
    // Define available actions
    availableActions = {
        GameAction::GATHER,
        GameAction::TAX,
        GameAction::BRIBE,
        GameAction::ARREST,
        GameAction::SANCTION,
        GameAction::COUP,
    };
    
    std::vector<std::string> actionNames = {
        "Gather (+1)",
        "Tax (+2)",
        "Bribe (-4)",
        "Arrest (+1)",
        "Sanction (-3)",
        "Coup (-7)"
    };
    
    actionButtons.resize(availableActions.size());
    actionButtonTexts.resize(availableActions.size());
    
    for (size_t i = 0; i < availableActions.size(); i++) {
        // Button
        actionButtons[i].setSize(sf::Vector2f(140, 40));
        actionButtons[i].setPosition(20, 160 + i * 50);
        actionButtons[i].setFillColor(buttonColor);
        
        // Button text
        actionButtonTexts[i].setString(actionNames[i]);
        if (fontLoaded) actionButtonTexts[i].setFont(font);
        actionButtonTexts[i].setCharacterSize(14);
        actionButtonTexts[i].setFillColor(textColor);
        
        // Center text in button
        sf::FloatRect textBounds = actionButtonTexts[i].getLocalBounds();
        actionButtonTexts[i].setPosition(
            20 + 70 - textBounds.width / 2,
            160 + i * 50 + 20 - textBounds.height / 2
        );
    }
    
    // Challenge/Block/Allow buttons (initially hidden)
    // challengeButton.setSize(sf::Vector2f(100, 35));
    // challengeButton.setPosition(400, 650);
    // challengeButton.setFillColor(sf::Color(220, 20, 60)); // Crimson
    
    // challengeButtonText.setString("Challenge");
    // if (fontLoaded) challengeButtonText.setFont(font);
    // challengeButtonText.setCharacterSize(12);
    // challengeButtonText.setFillColor(textColor);
    // challengeButtonText.setPosition(415, 660);
    
    blockButton.setSize(sf::Vector2f(100, 35));
    blockButton.setPosition(520, 650);
    blockButton.setFillColor(sf::Color(255, 140, 0)); // Dark orange
    
    blockButtonText.setString("Block");
    if (fontLoaded) blockButtonText.setFont(font);
    blockButtonText.setCharacterSize(12);
    blockButtonText.setFillColor(textColor);
    blockButtonText.setPosition(550, 660);
    
    allowButton.setSize(sf::Vector2f(100, 35));
    allowButton.setPosition(640, 650);
    allowButton.setFillColor(sf::Color(34, 139, 34)); // Forest green
    
    allowButtonText.setString("Allow");
    if (fontLoaded) allowButtonText.setFont(font);
    allowButtonText.setCharacterSize(12);
    allowButtonText.setFillColor(textColor);
    allowButtonText.setPosition(670, 660);
}

void GameGui::updatePlayerDisplay() {
    for (int i = 0; i < numPlayers; i++) {
        // Update colors based on current player
        if (i == game->get_turn()) {
            playersGui[i].playerCard.setFillColor(activePlayerColor);
        } else {
            playersGui[i].playerCard.setFillColor(inactivePlayerColor);
        }
        
        // Update coin and influence counts
        playersGui[i].coinsText.setString("Coins: " + std::to_string(game->get_players()[i]->get_coins()));
        //players[i].influenceText.setString("Cards: " + std::to_string(players[i].influence));
    }
}

void GameGui::updateCurrentPlayerDisplay() {
    currentPlayerText.setString("Current Player: " + game->get_players()[game->get_turn()]->get_name());
}

bool GameGui::isPointInButton(sf::Vector2i point, const sf::RectangleShape& button) {
    sf::FloatRect bounds = button.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(point));
}

void GameGui::handleMouseClick(sf::Vector2i mousePos) {
    //if (gamePhase == 0) { // Action selection phase
        for (size_t i = 0; i < actionButtons.size(); i++) {
            if (isPointInButton(mousePos, actionButtons[i])) {
                executeAction(availableActions[i]);
                return;
            }
        }
    //}
    
    // Handle block/allow buttons
    // if (gamePhase == 2) {
    //     // if (isPointInButton(mousePos, challengeButton)) {
    //     //     updateInfoPanel("Action challenged! Resolving...");
    //     //     gamePhase = 0;
    //     //     nextPlayer();
    //     //}
    //     if (isPointInButton(mousePos, blockButton)) {
    //         updateInfoPanel("Action blocked!");
    //         gamePhase = 0;
    //         nextPlayer();
    //     } else if (isPointInButton(mousePos, allowButton)) {
    //         updateInfoPanel("Action allowed.");
    //         gamePhase = 0;
    //         nextPlayer();
    //     }
    // }
}

void GameGui::handleMouseMove(sf::Vector2i mousePos) {
    // Handle button hover effects
    for (size_t i = 0; i < actionButtons.size(); i++) {
        if (isPointInButton(mousePos, actionButtons[i])) {
            actionButtons[i].setFillColor(buttonHoverColor);
        } else {
            actionButtons[i].setFillColor(buttonColor);
        }
    }
}

void GameGui::executeAction(GameAction action) {
    if (game->get_players().empty()) {
        std::cout << "Error: No players in game!" << std::endl;
        return;
    }
    std::string actionName;
    game->make_action();
    switch (action) {
        case GameAction::GATHER:
            // game->get_players()[game->get_turn()]->gather();
            // ///players[currentPlayer].coins += 1;
            // actionName = "Gather";
            // //gamePhase = 0;
            // nextPlayer();
            break;
    
            
        case GameAction::TAX:
            // game->get_players()[game->get_turn()]->tax();
            // //players[currentPlayer].coins += 2;
            // actionName = "Tax";
            // gamePhase = 2; // Others can block
            break;
            
        case GameAction::COUP:
        // game->get_players()[game->get_turn()]->coup();
        //     //players[currentPlayer].coins -= 7;
        //     actionName = "Coup";
        //     gamePhase = 1; // Target selection needed
            break;
            
        case GameAction::BRIBE:
        // game->get_players()[game->get_turn()]->bribe();
        //     //players[currentPlayer].coins += 3;
        //     actionName = "Bribe";
        //     gamePhase = 2; // Can be challenged
            break;
            
        case GameAction::ARREST:
        // game->get_players()[game->get_turn()]->arrest();
        //     //players[currentPlayer].coins -= 3;
        //     actionName = "Arrest";
        //     gamePhase = 2; // Can be challenged/blocked     
            break;
            
        case GameAction::SANCTION:
            // game->get_players()[game->get_turn()]->sanction();
            // actionName = "Sanction";
            // gamePhase = 2; 
            break;
            
        
    }
    
    updateInfoPanel(game->get_players()[game->get_turn()]->get_name() + " chose " + actionName);
    updatePlayerDisplay();
////checkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
    // if (gamePhase == 2) {
    //     phaseText.setString("Phase: Block Response");
    //     instructionText.setString("Other players can challenge or block:");
    // }
}

void GameGui::nextPlayer() {
    //currentPlayer = (currentPlayer + 1) % numPlayers;
    updateCurrentPlayerDisplay();
    updatePlayerDisplay();
    
    phaseText.setString("Phase: Action Selection");
    instructionText.setString("Choose an action:");
}

void GameGui::updateInfoPanel(const std::string& message) {
    std::string currentText = infoPanelText.getString();
    
    // Keep last few lines of log
    size_t lineCount = 0;
    size_t pos = currentText.length();
    while (pos > 0 && lineCount < 12) {
        pos = currentText.find_last_of('\n', pos - 1);
        if (pos == std::string::npos) break;
        lineCount++;
    }
    
    if (lineCount >= 12 && pos != std::string::npos) {
        currentText = currentText.substr(pos + 1);
    }
    
    infoPanelText.setString(currentText + "\n" + message);
}

void GameGui::draw() {
    window.clear(backgroundColor);
    
    // Draw UI elements
    window.draw(titleText);
    window.draw(currentPlayerText);
    window.draw(phaseText);
    window.draw(instructionText);
    
    // Draw players
    for (int i = 0; i < numPlayers; i++) {
        window.draw(playersGui[i].playerCard);
        window.draw(playersGui[i].nameText);
        window.draw(playersGui[i].coinsText);
        //window.draw(players[i].influenceText);
    }
    
    // Draw action buttons
    for (size_t i = 0; i < actionButtons.size(); i++) {
        window.draw(actionButtons[i]);
        window.draw(actionButtonTexts[i]);
    }
    
    // Draw info panel
    window.draw(infoPanel);
    window.draw(infoPanelText);
    
    // Draw challenge/block buttons if in appropriate phase
    // if (gamePhase == 2) {
    //     //window.draw(challengeButton);
    //     //window.draw(challengeButtonText);
    //     window.draw(blockButton);
    //     window.draw(blockButtonText);
    //     window.draw(allowButton);
    //     window.draw(allowButtonText);
    // }
    
    window.display();
}

void GameGui::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                handleMouseClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            handleMouseMove(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        }
    }
}

void GameGui::run() {
    while (window.isOpen()) {
        handleEvents();
        draw();
    }
}