#include "GameGui.hpp"
#include <iostream>
//#include <string>
#include <cmath>

#include "../Players/General.hpp"
#include "../Players/Judge.hpp"
#include "../Players/Merchant.hpp"
#include "../Players/Baron.hpp"
#include "../Players/Governor.hpp"
#include "../Players/Spy.hpp"
#include "../Players/PlayerFactory.hpp"


GameGui::GameGui(int playerCount) 
    : window(sf::VideoMode(1200, 800), "Coup - Main Game")
    , font()
    , fontLoaded(false)
    , numPlayers(playerCount)
    , game(&Game::instance())
    , rng(std::random_device{}())
    , waitingForBlock(false)
    , blockingPlayer(-1)
    , lastActionTarget(-1)
    
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
    roleNames = {"Governor", "Spy", "Baron", "General", "Judge", "Merchant"};

    std::cout << "About to initialize colors" << std::endl;
    initializeColors();
    
    std::cout << "About to initialize players" << std::endl;
    initializePlayers();
    
    std::cout << "About to initialize UI" << std::endl;
    initializeUI();
    
    std::cout << "About to initialize action buttons" << std::endl;
    initializeActionButtons();
    
    gamePhase = 0;
    targetPlayer = -1;
    isBribe = false;

    std::cout << "Constructor completed successfully" << std::endl;
    updateInfoPanel("Game Started! Player 1's turn.");
    
}

void GameGui::initializeColors() {
    backgroundColor = sf::Color(25, 35, 45);        // Dark blue
    panelColor = sf::Color(45, 55, 65);             // Lighter blue-gray
    activePlayerColor = sf::Color(100, 149, 237);   // Cornflower blue
    inactivePlayerColor = sf::Color(70, 80, 90);    // Gray
    coupedPlayerColor = sf::Color(255,0,0);         // Red
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

   std::vector<std::string> availableRoles(numPlayers);
    for (int i = 0; i < numPlayers; ++i) {
        availableRoles[i] = roleNames[rng() % roleNames.size()];
    }      
    std::shuffle(availableRoles.begin(), availableRoles.end(), rng);
    
    for (int i = 0; i < numPlayers; i++) {
        Player* newPlayer = nullptr;
        std::string playerName = "Player " + std::to_string(i + 1);
        std::string role = availableRoles[i];
        
        // Create player based on role
        //Player* newPlayer = PlayerFactory::createPlayer(role, playerName);
        newPlayer = PlayerFactory::createPlayer(role, playerName);
        
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
        playersGui[i].playerCard.setSize(sf::Vector2f(160, 140));
        playersGui[i].playerCard.setPosition(playersGui[i].position);
        playersGui[i].playerCard.setFillColor(inactivePlayerColor);
        
        // Player name
        playersGui[i].nameText.setString(game->get_players()[i]->get_name());
        if (fontLoaded) playersGui[i].nameText.setFont(font);
        playersGui[i].nameText.setCharacterSize(16);
        playersGui[i].nameText.setFillColor(textColor);
        playersGui[i].nameText.setStyle(sf::Text::Bold);
        playersGui[i].nameText.setPosition(playersGui[i].position.x + 10, playersGui[i].position.y + 10);
        
         // Role display
        std::string roleText = getRoleName(game->get_players()[i]);
        sf::Text roleDisplay;
        roleDisplay.setString("Role: " + roleText);
        if (fontLoaded) roleDisplay.setFont(font);
        roleDisplay.setCharacterSize(12);
        roleDisplay.setFillColor(sf::Color(100, 255, 100)); // Light green
        roleDisplay.setPosition(playersGui[i].position.x + 5, playersGui[i].position.y + 25);
        playersGui[i].roleText = roleDisplay;

        // Coins display
        playersGui[i].coinsText.setString("Coins: " + std::to_string(game->get_players()[i]->get_coins()));
        if (fontLoaded) playersGui[i].coinsText.setFont(font);
        playersGui[i].coinsText.setCharacterSize(14);
        playersGui[i].coinsText.setFillColor(sf::Color(255, 215, 0)); // Gold
        playersGui[i].coinsText.setPosition(playersGui[i].position.x + 10, playersGui[i].position.y + 40);
        
         // Status display (for sanctions, etc.)
        sf::Text statusDisplay;
        statusDisplay.setString(getPlayerStatus(game->get_players()[i]));
        if (fontLoaded) statusDisplay.setFont(font);
        statusDisplay.setCharacterSize(10);
        statusDisplay.setFillColor(sf::Color(255, 100, 100));
        statusDisplay.setPosition(playersGui[i].position.x + 5, playersGui[i].position.y + 65);
        playersGui[i].statusText = statusDisplay;
    }
    
    updatePlayerDisplay();
}
std::string GameGui::getRoleName(Player* player) {
    if (dynamic_cast<Governor*>(player)) return "Governor";
    if (dynamic_cast<Spy*>(player)) return "Spy";
    if (dynamic_cast<Baron*>(player)) return "Baron";
    if (dynamic_cast<General*>(player)) return "General";
    if (dynamic_cast<Judge*>(player)) return "Judge";
    if (dynamic_cast<Merchant*>(player)) return "Merchant";
    return "Citizen";
}


std::string GameGui::getPlayerStatus(Player* player) {
    std::string status = "";
    if (player->get_isSanction()) {
        status += "Sanctioned ";
    }
    if (!player->get_isActive()) {
        status += "Eliminated ";
    }
    return status;
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
        GameAction::INVEST,
        GameAction::REVEAL,
        GameAction::GATHER,
        GameAction::TAX,
        GameAction::BRIBE,
        GameAction::ARREST,
        GameAction::SANCTION,
        GameAction::COUP,
    };
    
    std::vector<std::string> actionNames = {
        "Invest (+3)",
        "Reveal (free)",
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
        } else if(!game->get_players()[i]->get_isActive()) {
            playersGui[i].playerCard.setFillColor(coupedPlayerColor);
        } else{
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
    else if (gamePhase == 2) { // Block response phase
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
    
    // Find  actual target 
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
    lastActionTarget = actualTargetIndex;
    
    std::string actionName;
    
    switch (pendingAction) {
         case GameAction::GATHER:
        break;
        case GameAction::TAX:
            break;
        case GameAction::BRIBE:
            break;
        case GameAction::COUP:
            actionName = "Coup";
            if (hasGeneralToBlock()) {
                waitingForBlock = true;
                lastAction = pendingAction;
                updateInfoPanel(currentPlayer->get_name() + " wants to coup " + target->get_name() + " - Generals can block!");
                gamePhase = 2;
                phaseText.setString("Phase: Block Response");
                instructionText.setString("Generals can block this coup:");
            }else{
                currentPlayer->coup(*target);
                actionName = "Coup";
                updateInfoPanel(currentPlayer->get_name() + " couped " + target->get_name());
                targetButtons.clear();
                targetButtonTexts.clear();
                gamePhase = 0;
                nextPlayer();
            }
            break;
            
       case GameAction::ARREST:
            waitingForBlock = true;
            lastAction = pendingAction;
            actionName = "Arrest";
            updateInfoPanel(currentPlayer->get_name() + " wants to " + actionName + " " + target->get_name() + " - Can be blocked!");
            gamePhase = 2;
            phaseText.setString("Phase: Block Response");
            instructionText.setString("Target or others can block:");
            break;
            
        case GameAction::SANCTION:
            waitingForBlock = true;
            lastAction = pendingAction;
            actionName = "Sanction";
            updateInfoPanel(currentPlayer->get_name() + " wants to " + actionName + " " + target->get_name() + " - Can be blocked!");
            gamePhase = 2;
            phaseText.setString("Phase: Block Response");
            instructionText.setString("Target or others can block:");
            break;
        case GameAction::REVEAL:{
            if(!dynamic_cast<Spy*>(currentPlayer)){
                updateInfoPanel("Only Spy can reveal!");
                return;
            }
            Spy* spy = dynamic_cast<Spy*>(currentPlayer);
            spy->reveal(*target);
            actionName = "reveal";
            updateInfoPanel(currentPlayer->get_name() + " revealed " + target->get_name());
            targetButtons.clear();
            targetButtonTexts.clear();
            gamePhase = 0;
            break;
        }
        default:
            break;
        
    }
    
    targetButtons.clear();
    targetButtonTexts.clear();
    updatePlayerDisplay();
}

bool GameGui::hasGeneralToBlock() {
    std::vector<Player*>& players = game->get_players();
    for (Player* p : players) {
        if (p->get_name() != players[game->get_turn()]->get_name() && dynamic_cast<General*>(p) && p->get_isActive() && p->get_coins() >= 5) {
            return true;
        }
    }
    return false;
}
bool GameGui::hasGovernorToBlock() {
    std::vector<Player*>& players = game->get_players();
    for (Player* p : players) {
        if (p->get_name() != players[game->get_turn()]->get_name() && 
            dynamic_cast<Governor*>(p) && 
            p->get_isActive()) {
            return true;
        }
    }
    return false;
}

bool GameGui::hasJudgeToBlock() {
    std::vector<Player*>& players = game->get_players();
    for (Player* p : players) {
        if (p->get_name() != players[game->get_turn()]->get_name() && 
            dynamic_cast<Judge*>(p) && 
            p->get_isActive()) {
            return true;
        }
    }
    return false;
}

void GameGui::handleBlock() {
    std::vector<Player*>& players = game->get_players();
    Player* currentPlayer = players[game->get_turn()];
    Player* target = players[targetPlayer];
    std::string blockMessage;
    switch (lastAction) {
        case GameAction::TAX:
            // Any Governor can block
            blockMessage = "Tax blocked by Governor!";
            // Reverse the tax effect
            // if (dynamic_cast<Governor*>(currentPlayer)) {
            //     currentPlayer->set_coins(currentPlayer->get_coins() - 3);
            // } else {
            //     currentPlayer->set_coins(currentPlayer->get_coins() - 2);
            // }
            break;
            
        case GameAction::BRIBE:
            // Any Judge can block and currentPlayer loses the 4 coins
             if (target) {
                // General pays 5 coins to block
                for (Player* p : players) {
                    if (hasJudgeToBlock()) {
                        blockMessage = p->get_name() + " (Judge) blocked Bribe! 4 coins lost!";
                        break;
                    }
                }
                // Bribe player still loses their 4 coins
                currentPlayer -> set_coins(currentPlayer->get_coins() - 4);
                
            }
            break;
            
        case GameAction::COUP:
            if (target) {
                // General pays 5 coins to block
                for (Player* p : players) {
                    if (dynamic_cast<General*>(p) && p->get_isActive() && p->get_coins() >= 5) {
                        p->set_coins(p->get_coins() - 5);
                        blockMessage = p->get_name() + " (General) blocked coup for 5 coins!";
                        break;
                    }
                }
                // Coup player still loses their 7 coins
                currentPlayer -> set_coins(currentPlayer->get_coins() - 7);
                
            }
            break;
            
        default:
            blockMessage = "Action blocked!";
            break;
    }
    
    updateInfoPanel(blockMessage);
    gamePhase = 0;
    targetPlayer = -1;
    waitingForBlock = false;
    nextPlayer();
    updatePlayerDisplay();
}

void GameGui::handleAllow() {
    std::vector<Player*>& players = game->get_players();
    Player* currentPlayer = players[game->get_turn()];
    Player* target = (lastActionTarget != -1) ? players[lastActionTarget] : nullptr;
    
    std::string actionName;
    
    switch (lastAction) {
        case GameAction::TAX:
            actionName = "Tax";
            currentPlayer->tax(); 
            updateInfoPanel("Tax executed - " + currentPlayer->get_name() + " gained coins!");
            break;

        case GameAction::BRIBE:
            isBribe = true;
            currentPlayer->bribe();
            actionName = "Bribe";
            // Player gets another turn - don't call nextPlayer()
            gamePhase = 0;
            targetPlayer = -1;
            waitingForBlock = false;
            phaseText.setString("Phase: Action Selection");
            instructionText.setString("Choose 2 more actions (Bribe bonus turn):");
            updateInfoPanel("Bribe executed - " + currentPlayer->get_name() + " gets another turn!");
            updatePlayerDisplay();
            return;
            
        case GameAction::ARREST://////checkkkkkkkkkkkkkkkkkkkk
            if (target) {
                //int oldCoins = target->get_coins();
                currentPlayer->arrest(*target);
                
                // Handle General defensive ability
                if (dynamic_cast<General*>(target)) {
                    updateInfoPanel(target->get_name() + " (General) defended against arrest!");
                } else if (dynamic_cast<Merchant*>(target)) {
                    updateInfoPanel(target->get_name() + " (Merchant) paid 2 coins to treasury instead!");
                } else {
                    updateInfoPanel("Arrest executed - " + target->get_name() + " lost 1 coin");
                }
                actionName = "Arrest";
            }
            break;

         case GameAction::SANCTION:
            if (target) {
                currentPlayer->sanction(*target);
                
                // Handle Judge defensive ability
                if (dynamic_cast<Judge*>(target)) {
                    updateInfoPanel(target->get_name() + " (Judge) made attacker pay extra coin!");
                } else {
                    updateInfoPanel("Sanction executed - " + target->get_name() + " is sanctioned!");
                }
                actionName = "Sanction";
            }
            break;

        case GameAction::COUP:
            if (target) {
                currentPlayer->coup(*target);
                updateInfoPanel("Coup executed - " + target->get_name() + " eliminated!");
                actionName = "Coup";
                //playersGui[lastActionTarget].playerCard.setFillColor(coupedPlayerColor);
            }
            break;
            
        default:
            break;
    }
    gamePhase = 0;
    targetPlayer = -1;
    waitingForBlock = false;
    nextPlayer();
    updatePlayerDisplay();
}

void GameGui::executeAction(GameAction action) {
    if (game->get_players().empty()) {
        std::cout << "Error: No players in game!" << std::endl;
        return;
    }
    Player* currentPlayer = game->get_players()[game->get_turn()];
    std::string actionName;

    switch (action) {
        case GameAction::GATHER:
         if (currentPlayer->get_isSanction()) {
                updateInfoPanel(currentPlayer->get_name() + " is sanctioned and cannot gather!");
                return;
            }
            currentPlayer->gather();
            actionName = "Gather (+1 coin)";
            updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
            gamePhase = 0;
            nextPlayer();
            break;
    
            
        case GameAction::TAX:
            if (currentPlayer->get_isSanction()) {
                updateInfoPanel(currentPlayer->get_name() + " is sanctioned and cannot tax!");
                return;
            }
            if (getRoleName(currentPlayer) == "Governor"){
                actionName = "Tax (+3 coins as Governor)";
            }
            else{
                actionName = "Tax (+2 coins)";
            }
           if (hasGovernorToBlock()) {
                waitingForBlock = true;
                lastAction = action;
                pendingAction = action;
                updateInfoPanel(currentPlayer->get_name() + " used " + actionName + " - Governors can block!");
                gamePhase = 2;
                phaseText.setString("Phase: Block Response");
                instructionText.setString("Governors can block this tax:");
            } else {
                // No Governors to block, execute immediately
                currentPlayer->tax();
                updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
                gamePhase = 0;
                nextPlayer();
            }
            break;
            
        case GameAction::BRIBE:
            if (currentPlayer->get_coins() < 4) {
                updateInfoPanel("Not enough coins for Bribe! (Need 4 coins)");
                return;
            }
             if (hasJudgeToBlock()) {
                waitingForBlock = true;
                lastAction = action;
                pendingAction = action;
                updateInfoPanel(currentPlayer->get_name() + " used " + actionName + " - Judge can block!");
                gamePhase = 2;
                phaseText.setString("Phase: Block Response");
                instructionText.setString("Judge can block this bribe:");
            } else {
                // No Judge to block, execute immediately
                currentPlayer->bribe();
                updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
                gamePhase = 0;
                nextPlayer();
            }
            
            break;
            
        case GameAction::ARREST:
            if(!currentPlayer->get_canArrest()){
                updateInfoPanel(currentPlayer->get_name() + "cannot arrest! choose other action!");
                return;
            }
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
            //gamePhase = 2; // Block/Allow phase
            phaseText.setString("Phase: Block Response");
            instructionText.setString("Other players can Block or allow:");
            break;
        
        case GameAction::INVEST:{
            
            if(!dynamic_cast<Baron*>(currentPlayer)){
                updateInfoPanel("Only Baron can invest!");
                return;
            }
            Baron* baron = dynamic_cast<Baron*>(currentPlayer);
            baron->invest();
            actionName = "Invest (+3 coin)";
            updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
            gamePhase = 0;
            nextPlayer();
            break;
        }
        case GameAction::REVEAL:{
            if(!dynamic_cast<Spy*>(currentPlayer)){
                updateInfoPanel("Only Spy can reveal!");
                return;
            }
            //Spy* spy = dynamic_cast<Spy*>(currentPlayer);
            pendingAction = action;
            actionName = "Reveal";
            updateInfoPanel(currentPlayer->get_name() + " wants to " + actionName + " - Choose target:");
            gamePhase = 1; // Target selection phase
            setupTargetSelection();
            break;
        }
        
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
    std::vector<Player*>& players = game->get_players();
    Player* currentPlayer = players[game->get_turn()];
    
    // Clear sanctions from previous turn
    if (currentPlayer->get_isSanction()) {
        currentPlayer->set_isSanction(false);
        updateInfoPanel(currentPlayer->get_name() + "'s sanction has been lifted!");
    }
    if (!currentPlayer->get_canArrest()) {
        currentPlayer->set_canArrest(true);
        updateInfoPanel(currentPlayer->get_name() + "'s arrest ban has been lifted!");
    }
    
    // Move to next active player
    do {
        if(isBribe){
            isBribe = false;
            break;
        }
        game->set_turn((game->get_turn() + 1) % numPlayers);
    } while (!players[game->get_turn()]->get_isActive());


    if(dynamic_cast<Merchant*>(players[game->get_turn()]) && players[game->get_turn()]->get_coins() > 2){
        players[game->get_turn()]->set_coins(players[game->get_turn()]->get_coins() + 1);
    }

    if(players[game->get_turn()]->get_coins() > 9){
        executeAction(GameAction::COUP);
    }
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
        window.draw(playersGui[i].roleText); 
        window.draw(playersGui[i].coinsText);
        window.draw(playersGui[i].statusText);
        
    }
    
    // Draw action buttons
    if (gamePhase == 0) {
        for (size_t i = 0; i < actionButtons.size(); i++) {
            window.draw(actionButtons[i]);
            window.draw(actionButtonTexts[i]);
        }
    }

    // Draw target selection buttons
    if (gamePhase == 1) {
        for (size_t i = 0; i < targetButtons.size(); i++) {
            window.draw(targetButtons[i]);
            window.draw(targetButtonTexts[i]);
        }
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