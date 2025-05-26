#include "GameGui.hpp"
#include <iostream>
//#include <string>
#include <cmath>

GameGui::GameGui(int playerCount) 
    : window(sf::VideoMode(1200, 800), "Coup - Main Game")
    , font()
    , fontLoaded(false)
    , numPlayers(playerCount)
    ,gamePhase(0)
    ,targetPlayer(-1)
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
    std::cout << "click \n";
    if (gamePhase == 0) { // Action selection phase
        for (size_t i = 0; i < actionButtons.size(); i++) {
            if (isPointInButton(mousePos, actionButtons[i])) {
                executeAction(availableActions[i]);
                return;
            }
        }
    }
    else if (gamePhase == 1) { // Target selection phase
        for (size_t i = 0; i < targetButtons.size(); i++) {
            if (isPointInButton(mousePos, targetButtons[i])) {
                executeTargetedAction(i);
                return;
            }
        }
    }
    else if (gamePhase == 2) { // Challenge/Block response phase
        if (isPointInButton(mousePos, blockButton)) {
            handleBlock();
        } else if (isPointInButton(mousePos, allowButton)) {
            handleAllow();
        }
    }
}

void GameGui::handleMouseMove(sf::Vector2i mousePos) {
    // Handle button hover effects
    if (gamePhase == 0) {
        for (size_t i = 0; i < actionButtons.size(); i++) {
            if (isPointInButton(mousePos, actionButtons[i])) {
                actionButtons[i].setFillColor(buttonHoverColor);
            } else {
                actionButtons[i].setFillColor(buttonColor);
            }
        }
    }
    
    // Handle target button hover effects (only in target selection phase)
    if (gamePhase == 1) {
        for (size_t i = 0; i < targetButtons.size(); i++) {
            if (isPointInButton(mousePos, targetButtons[i])) {
                targetButtons[i].setFillColor(buttonHoverColor);
            } else {
                targetButtons[i].setFillColor(buttonColor);
            }
        }
    }
    
    if (gamePhase == 2) {

        // Block button hover
        if (isPointInButton(mousePos, blockButton)) {
            blockButton.setFillColor(sf::Color(255, 165, 0)); // Orange hover
        } else {
            blockButton.setFillColor(sf::Color(255, 140, 0)); // Dark orange default
        }
        
        // Allow button hover
        if (isPointInButton(mousePos, allowButton)) {
            allowButton.setFillColor(sf::Color(50, 205, 50)); // Lime green hover
        } else {
            allowButton.setFillColor(sf::Color(34, 139, 34)); // Forest green default
        }
    }
}

void GameGui::executeTargetedAction(int targetIndex) {
    std::vector<Player*>& players = game->get_players();
    Player* currentPlayer = players[game->get_turn()];
    
    // Find the actual target player index
    int actualTargetIndex = -1;
    int validTargetCount = 0;
    
    for (int i = 0; i < numPlayers; i++) {//looks for the actual target
        if (i != game->get_turn() && players[i]->get_isActive()) {
            if (validTargetCount == targetIndex) {
                actualTargetIndex = i;
                break;
            }
            validTargetCount++;
        }
    }
    
    if (actualTargetIndex == -1) return;
    
    Player* target = players[actualTargetIndex];
    targetPlayer = actualTargetIndex;
    
    std::string actionName;
    
    switch (pendingAction) {
        case GameAction::COUP:
            actionName = "Coup";
            
            // currentPlayer->coup(*target);
            // actionName = "Coup";
            // updateInfoPanel(currentPlayer->get_name() + " couped " + target->get_name());
            // gamePhase = 0;
            // nextPlayer();
            break;
            
        case GameAction::ARREST:
        case GameAction::SANCTION:
            // These can be challenged and/or blocked
            actionName = (pendingAction == GameAction::ARREST) ? "Arrest" : "Sanction";
            // updateInfoPanel(currentPlayer->get_name() + " wants to " + actionName + " " + target->get_name());
            // gamePhase = 2; // Challenge/Block phase
            // phaseText.setString("Phase: Challenge/Block Response");
            // instructionText.setString("Target can block, others can challenge:");
            break;
            
        default:
            break;
        
    }
    
    updateInfoPanel(currentPlayer->get_name() + " wants to " + actionName + " " + target->get_name());
    gamePhase = 2; // Challenge/Block phase
    phaseText.setString("Phase: Block Response");
    instructionText.setString("Waiting for Block/Allow");

    // Clear target buttons
    targetButtons.clear();
    targetButtonTexts.clear();
    updatePlayerDisplay();
}

void GameGui::handleBlock() {
    std::vector<Player*>& players = game->get_players();
    Player* target = players[targetPlayer];
    
    updateInfoPanel(target->get_name() + " blocked the action!");
    gamePhase = 0;
    nextPlayer();
}

void GameGui::handleAllow() {
    std::vector<Player*>& players = game->get_players();
    Player* currentPlayer = players[game->get_turn()];
    
    std::string actionName;
    
    switch (pendingAction) {
        case GameAction::TAX:
            currentPlayer->tax();
            actionName = "Tax";
            break;
            
        case GameAction::BRIBE:
            currentPlayer->bribe();
            actionName = "Bribe";
            break;
            
        case GameAction::ARREST:
            if (targetPlayer != -1) {
                currentPlayer->arrest(*players[targetPlayer]);
                actionName = "Arrest";
            }
            break;
            
        case GameAction::COUP:
            if (targetPlayer != -1) {
                currentPlayer->coup(*players[targetPlayer]);
                actionName = "Caoup";
            }
            break;
            
        default:
            break;
    }
    updateInfoPanel("Action " + actionName + " was allowed and executed.");
    gamePhase = 0;
    targetPlayer = -1;
    nextPlayer();
}

void GameGui::executeAction(GameAction action) {
    if (game->get_players().empty()) {
        std::cout << "Error: No players in game!" << std::endl;
        return;
    }
    Player* currentPlayer = game->get_players()[game->get_turn()];
    std::string actionName;
    game->make_action();
    switch (action) {
        case GameAction::GATHER:
            currentPlayer->gather();
            actionName = "Gather (+1 coin)";
            updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
            gamePhase = 0;
            nextPlayer();
            break;
    
            
        case GameAction::TAX:
            pendingAction = action;
            actionName = "Tax (+2 coins)";
            updateInfoPanel(currentPlayer->get_name() + " declared " + actionName);
            gamePhase = 2; // Block/Allow phase
            phaseText.setString("Phase: Block Response");
            instructionText.setString("Other players can Block or allow:");
            break;
            
        case GameAction::BRIBE:
            if (currentPlayer->get_coins() < 4) {
                updateInfoPanel("Not enough coins for Bribe! (Need 4 coins)");
                return;
            }
            pendingAction = action;
            actionName = "Bribe (-4 coins)";
            updateInfoPanel(currentPlayer->get_name() + " declared " + actionName);
            gamePhase = 2; // Block/Allow phase
            phaseText.setString("Phase: Block Response");
            instructionText.setString("Other players can Block or allow:");
            break;
            
        case GameAction::ARREST:
            if (currentPlayer->get_coins() < 1) {
                updateInfoPanel("Not enough coins for Arrest!");
                return;
            }
            pendingAction = action;
            actionName = "Arrest";
            updateInfoPanel(currentPlayer->get_name() + " wants to use " + actionName + " - Choose target:");
            gamePhase = 1; // Target selection phase
            setupTargetSelection();
            break;
            
        case GameAction::SANCTION:
            if (currentPlayer->get_coins() < 3) {
                    updateInfoPanel("Not enough coins for Sanction! (Need 3 coins)");
                    return;
                }
            pendingAction = action;
            actionName = "Sanction";
            updateInfoPanel(currentPlayer->get_name() + " wants to use " + actionName + " - Choose target:");
            gamePhase = 1; // Target selection phase
            setupTargetSelection();    
            break;
            
        case GameAction::COUP:
            if (currentPlayer->get_coins() < 7) {
                updateInfoPanel("Not enough coins for Coup! (Need 7 coins)");
                return;
            }
            pendingAction = action;
            actionName = "Coup";
            updateInfoPanel(currentPlayer->get_name() + " wants to " + actionName + " - Choose target:");
            gamePhase = 1; // Target selection phase
            setupTargetSelection();
            gamePhase = 2; // Block/Allow phase
            phaseText.setString("Phase: Block Response");
            instructionText.setString("Other players can Block or allow:");
            break;
            
        
    }
    
    //updateInfoPanel(game->get_players()[game->get_turn()]->get_name() + " chose " + actionName);
    updatePlayerDisplay();

}
///checkkkkkkkkkkkkkkkkkkkkkk
void GameGui::setupTargetSelection() {
    phaseText.setString("Phase: Target Selection");
    instructionText.setString("Choose target player:");
    
    // Create target selection buttons
    targetButtons.clear();
    targetButtonTexts.clear();
    
    std::vector<Player*>& players = game->get_players();
    int currentTurn = game->get_turn();
    
    for (int i = 0; i < numPlayers; i++) {
        if (i != currentTurn && players[i]->get_isActive()) {
            sf::RectangleShape button;
            button.setSize(sf::Vector2f(120, 30));
            button.setPosition(400 + (targetButtons.size() * 130), 650);
            button.setFillColor(buttonColor);
            targetButtons.push_back(button);
            
            sf::Text buttonText;
            buttonText.setString(players[i]->get_name());
            if (fontLoaded) buttonText.setFont(font);
            buttonText.setCharacterSize(12);
            buttonText.setFillColor(textColor);
            buttonText.setPosition(410 + (targetButtonTexts.size() * 130), 660);
            targetButtonTexts.push_back(buttonText);
        }
    }
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
        
    }
    
    // Draw action buttons
    for (size_t i = 0; i < actionButtons.size(); i++) {
        window.draw(actionButtons[i]);
        window.draw(actionButtonTexts[i]);
    }
    
    // Draw info panel
    window.draw(infoPanel);
    window.draw(infoPanelText);
    
    if (gamePhase == 2) {
        window.draw(blockButton);
        window.draw(blockButtonText);
        window.draw(allowButton);
        window.draw(allowButtonText);
    }
    
    window.display();
}

void GameGui::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            std::vector<Player*>& players = game->get_players();
            for(Player* p : players) {
                delete p;
            }
            players.clear();
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