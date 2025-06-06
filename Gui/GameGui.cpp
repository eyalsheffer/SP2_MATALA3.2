#include "GameGui.hpp"
#include <iostream>
//#include <string>
#include <cmath>
#include <sstream>

#include "../Players/General.hpp"
#include "../Players/Judge.hpp"
#include "../Players/Merchant.hpp"
#include "../Players/Baron.hpp"
#include "../Players/Governor.hpp"
#include "../Players/Spy.hpp"
#include "../Players/PlayerFactory.hpp"
#include "../GameAction.hpp"


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
    , gameEnded(false)  
    , winnerName("")
    
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
    //isBribe = false;

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
        newPlayer = PlayerFactory::createPlayer(role, *game, playerName);
        //
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

       
        // Coins display - only show for current player initially
        if (i == game->get_turn()) {
            playersGui[i].coinsText.setString("Coins: " + std::to_string(game->get_players()[i]->get_coins()));
        } else {
            playersGui[i].coinsText.setString("Coins: ???");
        }
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

    // Current blocker highlight (add after existing UI setup)
    currentBlockerHighlight.setSize(sf::Vector2f(170, 150));
    currentBlockerHighlight.setFillColor(sf::Color(255, 255, 0, 100)); // Semi-transparent yellow
    currentBlockerHighlight.setOutlineThickness(3);
    currentBlockerHighlight.setOutlineColor(sf::Color(255, 215, 0)); // Gold outline
    
    // Current blocker prompt
    currentBlockerPrompt.setString("");
    if (fontLoaded) currentBlockerPrompt.setFont(font);
    currentBlockerPrompt.setCharacterSize(18);
    currentBlockerPrompt.setFillColor(sf::Color(255, 215, 0)); // Gold
    currentBlockerPrompt.setStyle(sf::Text::Bold);
    currentBlockerPrompt.setPosition(400, 580);

      // Victory screen elements
    victoryOverlay.setSize(sf::Vector2f(600, 400));
    victoryOverlay.setPosition(300, 200);
    victoryOverlay.setFillColor(sf::Color(0, 0, 0, 200)); // Semi-transparent black
    victoryOverlay.setOutlineThickness(5);
    victoryOverlay.setOutlineColor(sf::Color(255, 215, 0)); // Gold border
    
    victoryTitleText.setString("GAME OVER!");
    if (fontLoaded) victoryTitleText.setFont(font);
    victoryTitleText.setCharacterSize(48);
    victoryTitleText.setFillColor(sf::Color(255, 215, 0)); // Gold
    victoryTitleText.setStyle(sf::Text::Bold);
    victoryTitleText.setPosition(450, 250);
    
    winnerText.setString("");
    if (fontLoaded) winnerText.setFont(font);
    winnerText.setCharacterSize(32);
    winnerText.setFillColor(sf::Color::White);
    winnerText.setStyle(sf::Text::Bold);
    winnerText.setPosition(450, 320);
    
    resetButton.setSize(sf::Vector2f(200, 60));
    resetButton.setPosition(500, 450);
    resetButton.setFillColor(sf::Color(34, 139, 34)); // Forest green
    resetButton.setOutlineThickness(2);
    resetButton.setOutlineColor(sf::Color::White);
    
    resetButtonText.setString("PLAY AGAIN");
    if (fontLoaded) resetButtonText.setFont(font);
    resetButtonText.setCharacterSize(20);
    resetButtonText.setFillColor(sf::Color::White);
    resetButtonText.setStyle(sf::Text::Bold);
    resetButtonText.setPosition(540, 470);

    // Always-visible reset button
    alwaysResetButton.setSize(sf::Vector2f(120, 40));
    alwaysResetButton.setPosition(1060, 680);
    defaultResetColor = sf::Color(220, 20, 60);  // Crimson
    hoverResetColor = sf::Color(255, 69, 0);     // Red-orange hover
    alwaysResetButton.setFillColor(defaultResetColor);
    alwaysResetButton.setOutlineThickness(2);
    alwaysResetButton.setOutlineColor(sf::Color::White);
    
    alwaysResetText.setString("RESET");
    if (fontLoaded) alwaysResetText.setFont(font);
    alwaysResetText.setCharacterSize(14);
    alwaysResetText.setFillColor(sf::Color::White);
    alwaysResetText.setStyle(sf::Text::Bold);
    
    // Center text in button
    sf::FloatRect textBounds = alwaysResetText.getLocalBounds();
    alwaysResetText.setPosition(
        1060 + 60 - textBounds.width / 2,   // Center horizontally
        680 + 20 - textBounds.height / 2    // Center vertically
    );
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
        GameAction::UNIQE,
    };
    
    std::vector<std::string> actionNames = {

        "Gather (+1)",
        "Tax (+2)",
        "Bribe (-4)",
        "Arrest (+1)",
        "Sanction (-3)",
        "Coup (-7)",
        "Invest (+3)",
        "Reveal (free)"
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

    updateActionButtonVisibility();
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
        
        // Update coin display based on visibility rules
        if (i == game->get_turn() || 
            std::find(revealedPlayers.begin(), revealedPlayers.end(), i) != revealedPlayers.end()) {
            playersGui[i].coinsText.setString("Coins: " + std::to_string(game->get_players()[i]->get_coins()));
        } else {
            playersGui[i].coinsText.setString("Coins: ???");
        }
        
    }
}
void GameGui::updateActionButtonTexts() {
    Player* currentPlayer = game->get_players()[game->get_turn()];
    
    std::vector<std::string> actionNames = {
        "Gather (+1)",
        "Tax (+2)",
        "Bribe (-4)",
        "Arrest (+1)",
        "Sanction (-3)",
        "Coup (-7)",
        "", 
    };
    
    // Update the unique action button text based on current player's role
    if (dynamic_cast<Baron*>(currentPlayer)) {
        actionNames[6] = "Invest (+3)";
    } else if (dynamic_cast<Spy*>(currentPlayer)) {
        actionNames[6] = "Reveal (free)";
    } else {
        actionNames[6] = "No Ability";
    }
    
    // Update all button texts
    for (size_t i = 0; i < actionButtonTexts.size() && i < actionNames.size(); i++) {
        actionButtonTexts[i].setString(actionNames[i]);
        
        // Recenter text in button
        sf::FloatRect textBounds = actionButtonTexts[i].getLocalBounds();
        actionButtonTexts[i].setPosition(
            20 + 70 - textBounds.width / 2,
            160 + i * 50 + 20 - textBounds.height / 2
        );
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
    // Check for always-visible reset button
    if (isPointInButton(mousePos, alwaysResetButton)) {
        resetGame();
        return;
    }
    if (gameEnded) {
        if (isPointInResetButton(mousePos)) {
            resetGame();
        }
        return; // Don't process other clicks during victory screen
    }

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
     if (isPointInButton(mousePos, alwaysResetButton)) {
        alwaysResetButton.setFillColor(hoverResetColor);
    } else {
        alwaysResetButton.setFillColor(defaultResetColor);
    }
    
     // Handle victory screen reset button hover
    if (gameEnded) {
        if (isPointInResetButton(mousePos)) {
            resetButton.setFillColor(sf::Color(50, 205, 50)); // Lime green hover
        } else {
            resetButton.setFillColor(sf::Color(34, 139, 34)); // Forest green default
        }
        return; // Don't process other hover effects during victory screen
    }
      if (gamePhase == 0) {
        Player* currentPlayer = game->get_players()[game->get_turn()];
        
        for (size_t i = 0; i < actionButtons.size(); i++) {
            // Check if action is available
            bool isAvailable = true;
            
            switch (availableActions[i]) {
                case GameAction::GATHER:
                    isAvailable = !currentPlayer->get_isSanction();
                    break;
                    
                case GameAction::TAX:
                    isAvailable = !currentPlayer->get_isSanction();
                    break;
                    
                case GameAction::BRIBE:
                    isAvailable = currentPlayer->get_coins() >= 4;
                    break;
                    
                case GameAction::ARREST:
                    isAvailable = currentPlayer->get_canArrest();
                     if (isAvailable) {
                    // Check if there are any valid arrest targets
                    std::vector<Player*>& players = game->get_players();
                    bool hasValidTarget = false;
                    
                    for (int j = 0; j < numPlayers; j++) {
                        if (j != game->get_turn() && players[j]->get_isActive()) {
                            if (isValidArrestTarget(players[j])) {
                                hasValidTarget = true;
                                break;
                            }
                        }
                    }
                    
                    isAvailable = hasValidTarget;
                }
                break;
                    
                case GameAction::SANCTION:{
                    std::vector<Player*>& players = game->get_players();
                    bool hasValidTarget = false;
                    
                    for (int j = 0; j < numPlayers; j++) {
                        if (j != game->get_turn() && players[j]->get_isActive()) {
                            if (isValidSanctionTarget(players[j])) {
                                hasValidTarget = true;
                                break;
                            }
                        }
                    }
                    isAvailable = hasValidTarget;  
                    break;
                }       
                case GameAction::COUP:
                    isAvailable = currentPlayer->get_coins() >= 7;
                    break;
                    
                case GameAction::UNIQE:
                    // Check availability based on player role
                    if (dynamic_cast<Baron*>(currentPlayer)) {
                        // Baron's Invest ability
                        isAvailable = currentPlayer->get_coins() >= 3;
                    } else if (dynamic_cast<Spy*>(currentPlayer)) {
                        // Spy's Reveal ability
                        isAvailable = true; // Spy can always try to reveal
                    } else {
                        // Other roles have no unique ability
                        isAvailable = false;
                    }
                    break;
                    
                default:
                    isAvailable = true;
                    break;
            }
            
            // Apply hover effect only if action is available
            if (isAvailable) {
                if (isPointInButton(mousePos, actionButtons[i])) {
                    actionButtons[i].setFillColor(buttonHoverColor);
                } else {
                    actionButtons[i].setFillColor(buttonColor);
                }
            } else {
                // Keep grayed out appearance for unavailable actions
                actionButtons[i].setFillColor(sf::Color(50, 50, 50, 100));
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
    
    // Find actual target 
    int actualTargetIndex = -1;
    int validTargetCount = 0;
    
    for (int i = 0; i < numPlayers; i++) {
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
    
    int temp = lastPlayer;

    std::string actionName;
    
    switch (pendingAction) {
        case GameAction::COUP:
            try {
                // Execute action first
                lastPlayer = game->get_turn();
                currentPlayer->coup(*target);
                actionName = "Coup";
                coupTarget = target;
                // Check for blocking after execution
                if (players[lastPlayer]->get_lastAction() == GameAction::COUP && hasGeneralToBlock()) {
                    waitingForBlock = true;
                    lastAction = pendingAction;
                    updateInfoPanel(currentPlayer->get_name() + " couped " + target->get_name() + " - Generals can block!");
                    startBlockingSequence();
                } else {
                    // No Generals to block, action stays executed
                    updateInfoPanel(currentPlayer->get_name() + " couped " + target->get_name());
                    // Check for winner after coup
                    checkForWinner();
                    if (gameEnded) return;
                    targetButtons.clear();
                    targetButtonTexts.clear();
                    gamePhase = 0;
                    //nextPlayer();
                }
            } catch (const std::exception& e) {
                lastPlayer = temp;
                updateInfoPanel("Error executing Coup: " + std::string(e.what()));
                targetButtons.clear();
                targetButtonTexts.clear();
                gamePhase = 0;
                return;
            }
            break;
            
        case GameAction::ARREST:
            // Validate arrest target before execution
            if (!isValidArrestTarget(target)) {
                std::string reason = "";
                if (target->get_lastArrested()) {
                    reason = "they were arrested last turn";
                } else if (target->get_coins() == 0) {
                    reason = "they have no coins to lose";
                } else if (dynamic_cast<Merchant*>(target) && target->get_coins() < 2) {
                    reason = "they don't have enough coins to pay the merchant penalty";
                }
                
                updateInfoPanel("Cannot arrest " + target->get_name() + " - " + reason + "!");
                targetButtons.clear();
                targetButtonTexts.clear();
                gamePhase = 0;
                return;
            }
            
            try {
                // Execute action first
                lastPlayer = game->get_turn();
                currentPlayer->arrest(*target);
                actionName = "Arrest";

                std::vector<Player*>& allPlayers = game->get_players();
                for (Player* p : allPlayers) {
                    if (p != target) {
                        p->set_lastArrested(false);
                    }
                }
                target->set_lastArrested(true);
                
                // Handle role-specific arrests
                if (dynamic_cast<General*>(target)) {
                    updateInfoPanel(target->get_name() + " (General) defended against arrest!");
                } else if (dynamic_cast<Merchant*>(target)) {
                    updateInfoPanel(target->get_name() + " (Merchant) paid 2 coins to treasury instead!");
                } else {
                    updateInfoPanel("Arrest executed - " + target->get_name() + " lost 1 coin");
                }
                
                // No blocking allowed for ARREST
                targetButtons.clear();
                targetButtonTexts.clear();
                gamePhase = 0;
                //nextPlayer();
            } catch (const std::exception& e) {
                updateInfoPanel("Error executing Arrest: " + std::string(e.what()));
                lastPlayer = temp;
                targetButtons.clear();
                targetButtonTexts.clear();
                gamePhase = 0;
                return;
            }
            break;
            
        case GameAction::SANCTION:
            {
                int requiredCoins = 3; // Default cost
                if (dynamic_cast<Judge*>(target)) {
                    requiredCoins = 4; // Extra cost for sanctioning a Judge
                }
                if (currentPlayer->get_coins() < requiredCoins) {
                    updateInfoPanel("Not enough coins to sanction " + target->get_name() + "! (Need " + std::to_string(requiredCoins) + " coins)");
                    targetButtons.clear();
                    targetButtonTexts.clear();
                    gamePhase = 0;
                    return;
                }
                
                try {
                    // Execute action first
                    lastPlayer = game->get_turn();
                    currentPlayer->sanction(*target);
                    actionName = "Sanction";
                    
                    // Handle role-specific defensive abilities
                    if (dynamic_cast<Judge*>(target)) {
                        updateInfoPanel(target->get_name() + " (Judge) made attacker pay extra coin!");
                    } else {
                        updateInfoPanel("Sanction executed - " + target->get_name() + " is sanctioned!");
                    }
                    
                    // No blocking allowed for SANCTION
                    targetButtons.clear();
                    targetButtonTexts.clear();
                    gamePhase = 0;
                    //nextPlayer();
                } catch (const std::exception& e) {
                    updateInfoPanel("Error executing Sanction: " + std::string(e.what()));
                    lastPlayer = temp;
                    targetButtons.clear();
                    targetButtonTexts.clear();
                    gamePhase = 0;
                    return;
                }
            }
            break;
            
        case GameAction::UNIQE:
            // Handle role-specific unique abilities that require targets
            if(dynamic_cast<Spy*>(currentPlayer)){
                // Spy's Reveal ability
                try {
                    Spy* spy = dynamic_cast<Spy*>(currentPlayer);
                    spy->uniqe(*target);
                    revealedPlayers.push_back(actualTargetIndex);
                    actionName = "reveal";
                    updateInfoPanel(currentPlayer->get_name() + " revealed " + target->get_name());
                    targetButtons.clear();
                    targetButtonTexts.clear();
                    gamePhase = 0;
                    //nextPlayer();
                } catch (const std::exception& e) {
                    updateInfoPanel("Error executing Reveal: " + std::string(e.what()));
                    targetButtons.clear();
                    targetButtonTexts.clear();
                    gamePhase = 0;
                    return;
                }
            }
            else if(dynamic_cast<Baron*>(currentPlayer)){
                // Baron's Invest doesn't need a target
                updateInfoPanel("Baron's Invest doesn't require a target!");
                targetButtons.clear();
                targetButtonTexts.clear();
                gamePhase = 0;
                return;
            }
            else {
                updateInfoPanel(getRoleName(currentPlayer) + " has no unique ability that requires a target!");
                targetButtons.clear();
                targetButtonTexts.clear();
                gamePhase = 0;
                return;
            }
            break;
            
        default:
            break;
    }
    
    targetButtons.clear();
    targetButtonTexts.clear();
    updatePlayerDisplay();
}

bool GameGui::hasGeneralToBlock() {
   std::vector<Player*>& players = game->get_players();
   //int temp_turn = getActualCurrentIndex();
    eligibleBlockers.clear();
    
    for (int i = 0; i < static_cast<int>(players.size()); i++) {
        Player* p = players[i];
        if (p->get_name() != players[lastPlayer]->get_name() && 
            dynamic_cast<General*>(p) && 
            p->get_isActive() && 
            p->get_coins() >= 5) {
            eligibleBlockers.push_back(i);
        }
    }
    return !eligibleBlockers.empty();
}
bool GameGui::hasGovernorToBlock() {
    std::vector<Player*>& players = game->get_players();
   //int temp_turn = getActualCurrentIndex();
    eligibleBlockers.clear();
    
    for (int i = 0; i < static_cast<int>(players.size()); i++) {
        Player* p = players[i];
        if (p->get_name() != players[lastPlayer]->get_name() && 
            dynamic_cast<Governor*>(p) && 
            p->get_isActive()) { 
            eligibleBlockers.push_back(i);
        }
    }
    return !eligibleBlockers.empty();
}

bool GameGui::hasJudgeToBlock() {
    std::vector<Player*>& players = game->get_players();
    //int temp_turn = getActualCurrentIndex();
    eligibleBlockers.clear();
    
    for (int i = 0; i < static_cast<int>(players.size()); i++) {
        Player* p = players[i];
        if (p->get_name() != players[lastPlayer]->get_name() && 
            dynamic_cast<Judge*>(p) && 
            p->get_isActive()) {
            eligibleBlockers.push_back(i);
        }
    }
    return !eligibleBlockers.empty();
}



bool GameGui::isValidArrestTarget(Player* target) {
    // Cannot arrest if target was arrested last turn
    if (target->get_lastArrested()) {
        return false;
    }
    
    // Cannot arrest if target has no money 
    if (target->get_coins() == 0) {
        return false; // Regular player with 0 coins can't lose a coin
    }
    
    // Special case: Merchant needs at least 2 coins to pay the penalty instead of losing 1
    if (dynamic_cast<Merchant*>(target) && target->get_coins() < 2) {
        return false;
    }
    
    return true;
}

bool GameGui::isValidSanctionTarget(Player* target){
    Player* currentPlayer = game->get_players()[game->get_turn()];
    if(currentPlayer->get_coins() < 3){
        return false;
    }

    if (dynamic_cast<Judge*>(target) && currentPlayer->get_coins() < 4) {
        return false;
    }
    return true;

}

void GameGui::startBlockingSequence() {
    if (!eligibleBlockers.empty()) {
        currentBlockerIndex = 0;
        showCurrentBlockerOption();
    }
}

void GameGui::showCurrentBlockerOption() {
    if (currentBlockerIndex < static_cast<int>(eligibleBlockers.size())) {
        std::vector<Player*>& players = game->get_players();
        int blockerPlayerIndex = eligibleBlockers[currentBlockerIndex];
        currentBlockerName = players[blockerPlayerIndex]->get_name();
        
        // Highlight the current blocker's card
        currentBlockerHighlight.setPosition(
            playersGui[blockerPlayerIndex].position.x - 5, 
            playersGui[blockerPlayerIndex].position.y - 5
        );

        std::string actionName;
        std::string blockerRole;
        
        switch (lastAction) {
            case GameAction::TAX:
                actionName = "Tax";
                blockerRole = "Governor";
                break;
            case GameAction::BRIBE:
                actionName = "Bribe";
                blockerRole = "Judge";
                break;
            case GameAction::COUP:
                actionName = "Coup";
                blockerRole = "General";
                break;
            default:
                actionName = "Unknown";
                blockerRole = "Unknown";
                break;
        }
        
        // Update the prominent blocker prompt
        std::string promptText = ">>> " + currentBlockerName + " (" + blockerRole + ") <<<\n";
        promptText += "Can block " + actionName;
        currentBlockerPrompt.setString(promptText);
        
        // Center the prompt text
        sf::FloatRect textBounds = currentBlockerPrompt.getLocalBounds();
        currentBlockerPrompt.setPosition(
            600 - textBounds.width / 2,  // Center horizontally
            520                          // Fixed vertical position
        );
        
        updateInfoPanel(currentBlockerName + " (" + blockerRole + ") can block " + actionName + " - Choose:");
        instructionText.setString(currentBlockerName + " can block or allow this action:");
        gamePhase = 2;
    } else {
        // All blockers have decided to allow, action remains executed
        currentBlockerPrompt.setString("");
        gamePhase = 0;
        targetPlayer = -1;
        waitingForBlock = false;
        eligibleBlockers.clear();
        phaseText.setString("Phase: Action Selection");
        instructionText.setString("Choose an action:");
        
        // For bribe, don't advance turn since player gets another turn
        if (lastAction != GameAction::BRIBE) {
            //nextPlayer();
        }
        
        updatePlayerDisplay();
        updateActionButtonVisibility();
    }
}

void GameGui::handleBlock() {
    std::vector<Player*> players = game->get_players();
    Player* blocker = players[eligibleBlockers[currentBlockerIndex]];
    //int temp_turn = getActualCurrentIndex();
    Player* currentPlayer = players[lastPlayer]; 
    std::string blockMessage;
    
    try {
        // Call blocker's uniqe() method based on their type
        // Check if blocker is Baron (who doesn't need a target)
        if (dynamic_cast<Baron*>(blocker)) {
            blocker->uniqe(); // Baron invests, no target needed
        }else if (dynamic_cast<General*>(blocker))
        {
           blocker->uniqe(*players[lastPlayer], *coupTarget);
           game->set_turn(lastPlayer);
           game->turn_manager();

        }else if (dynamic_cast<Judge*>(blocker))//the turn didnt move yet
        {
            blocker->uniqe(*players[game->get_turn()]);
        }
        
         else {
            blocker->uniqe(*currentPlayer); // Other players block the current player
        }
        
        switch (lastAction) {
            case GameAction::TAX:
                blockMessage = currentBlockerName + " (Governor) blocked Tax!";
                break;
            case GameAction::BRIBE:
                blockMessage = currentBlockerName + " (Judge) blocked Bribe!";
                break;
            case GameAction::COUP:
                blockMessage = currentBlockerName + " (General) blocked Coup!";
                break;
            default:
                blockMessage = currentBlockerName + " blocked the action!";
                break;
        }
        
        updateInfoPanel(blockMessage);
        
        // Action was blocked, end turn
        currentBlockerPrompt.setString("");
        gamePhase = 0;
        targetPlayer = -1;
        waitingForBlock = false;
        eligibleBlockers.clear();
        phaseText.setString("Phase: Action Selection");
        instructionText.setString("Choose an action:");
        
        updatePlayerDisplay();
        updateActionButtonVisibility();
        
    } catch (const std::exception& e) {
        // Block failed due to runtime exception
        blockMessage = currentBlockerName + " failed to block - " + std::string(e.what());
        updateInfoPanel(blockMessage);
        
        // Continue to next blocker or execute action
        currentBlockerIndex++;
        showCurrentBlockerOption();
    }

}

void GameGui::handleAllow() {
     //std::vector<Player*>& players = game->get_players();
     updateInfoPanel(currentBlockerName + " allows the action.");
    
    // Move to next blocker
    currentBlockerIndex++;
    showCurrentBlockerOption();
    
}


void GameGui::executeAction(GameAction action) {
   if (game->get_players().empty()) {
        std::cout << "Error: No players in game!" << std::endl;
        return;
    }
    Player* currentPlayer = game->get_players()[game->get_turn()];
    std::string actionName;
    int temp = lastPlayer;// Backup lastPlayer in case gather() throws

    switch (action) {
        case GameAction::GATHER:
            if (currentPlayer->get_isSanction()) {
                updateInfoPanel(currentPlayer->get_name() + " is sanctioned and cannot gather!");
                return;
            }
            try {
                lastPlayer = game->get_turn();
                currentPlayer->gather();
                actionName = "Gather (+1 coin)";
                updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
                gamePhase = 0;
                
                // No blocking allowed for GATHER
                //nextPlayer();
                updateActionButtonVisibility();
            } catch (const std::exception& e) {
                updateInfoPanel("Error executing Gather: " + std::string(e.what()));
                lastPlayer = temp;
                return;
            }
            break;
    
        case GameAction::TAX:
            if (currentPlayer->get_isSanction()) {
                updateInfoPanel(currentPlayer->get_name() + " is sanctioned and cannot tax!");
                return;
            }
            
            try {
                // Execute action first
                lastPlayer = game->get_turn(); 
                currentPlayer->tax();
                
                if (getRoleName(currentPlayer) == "Governor"){
                    actionName = "Tax (+3 coins as Governor)";
                } else {
                    actionName = "Tax (+2 coins)";
                }
                
                // Check for blocking after execution
                if (hasGovernorToBlock()) {
                    waitingForBlock = true;
                    lastAction = action;
                    gamePhase = 2;
                    pendingAction = action;
                    updateInfoPanel(currentPlayer->get_name() + " used " + actionName + " - Governors can block!");
                    startBlockingSequence();
                } else {
                    // No Governors to block, action stays executed
                    updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
                    gamePhase = 0;
                    updateActionButtonVisibility();
                    //nextPlayer();
                }
            } catch (const std::exception& e) {
                updateInfoPanel("Error executing Tax: " + std::string(e.what()));
                lastPlayer = temp;
                return;
            }
            break;
            
        case GameAction::BRIBE:
            if (currentPlayer->get_coins() < 4) {
                updateInfoPanel("Not enough coins for Bribe! (Need 4 coins)");
                return;
            }
            
            try {
                // Execute action first
                lastPlayer = game->get_turn();
                currentPlayer->bribe();
                
                // Check for blocking after execution
                if (hasJudgeToBlock()) {
                    waitingForBlock = true;
                    lastAction = action;
                    pendingAction = action;
                    updateInfoPanel(currentPlayer->get_name() + " used Bribe - Judges can block!");
                    startBlockingSequence();
                } else {
                    // No Judge to block, action stays executed
                    updateInfoPanel(currentPlayer->get_name() + " used Bribe - gets another turn!");
                    gamePhase = 0;
                    // Don't call nextPlayer() for bribe - player gets another turn
                }
                updateActionButtonVisibility();
            } catch (const std::exception& e) {
                updateInfoPanel("Error executing Bribe: " + std::string(e.what()));
                lastPlayer = temp;
                return;
            }
            break;
            
        case GameAction::ARREST:
            if(!currentPlayer->get_canArrest()){
                updateInfoPanel(currentPlayer->get_name() + " cannot arrest! choose other action!");
                return;
            }

            // Check if there are any valid arrest targets
            {
                std::vector<Player*>& players = game->get_players();
                bool hasValidTarget = false;
                
                for (int i = 0; i < numPlayers; i++) {
                    if (i != game->get_turn() && players[i]->get_isActive()) {
                        if (isValidArrestTarget(players[i])) {
                            hasValidTarget = true;
                            break;
                        }
                    }
                }
                
                if (!hasValidTarget) {
                    updateInfoPanel("No valid arrest targets available! Choose another action.");
                    return;
                }
            }
            
            pendingAction = action;
            actionName = "Arrest";
            updateInfoPanel(currentPlayer->get_name() + " wants to use " + actionName + " - Choose target:");
            gamePhase = 1; // Target selection phase
            setupTargetSelection();
            updateActionButtonVisibility();
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
            updateActionButtonVisibility();  
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
            phaseText.setString("Phase: Block Response");
            instructionText.setString("Other players can Block or allow:");
            updateActionButtonVisibility();
            break;
        
        case GameAction::UNIQE:{
            // Handle role-specific unique abilities
            if(dynamic_cast<Baron*>(currentPlayer)){
                // Baron's Invest ability
                if (currentPlayer->get_coins() < 3) {
                    updateInfoPanel("Not enough coins for Invest! (Need 3 coins)");
                    return;
                }
                
                try {
                    Baron* baron = dynamic_cast<Baron*>(currentPlayer);
                    baron->uniqe();
                    actionName = "Invest (+3 coin)";
                    updateInfoPanel(currentPlayer->get_name() + " used " + actionName);
                    gamePhase = 0;
                    //nextPlayer();
                    updateActionButtonVisibility();
                } catch (const std::exception& e) {
                    updateInfoPanel("Error executing Invest: " + std::string(e.what()));
                    return;
                }
            }
            else if(dynamic_cast<Spy*>(currentPlayer)){
                // Spy's Reveal ability
                pendingAction = action;
                actionName = "Reveal";
                updateInfoPanel(currentPlayer->get_name() + " wants to " + actionName + " - Choose target:");
                gamePhase = 1; // Target selection phase
                setupTargetSelection();
                updateActionButtonVisibility();
            }
            else {
                // Player role has no unique ability
                updateInfoPanel(getRoleName(currentPlayer) + " has no unique ability!");
                return;
            }
            break;
        }
        case GameAction::NONE:
            break;
    }   
    updatePlayerDisplay();
}

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

            if (pendingAction == GameAction::ARREST && !isValidArrestTarget(players[i])) {
                continue;
            }
            if (pendingAction == GameAction::SANCTION && !isValidSanctionTarget(players[i])) {
                continue;
            }

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


void GameGui::updateInfoPanel(const std::string& message) {
    std::string currentText = infoPanelText.getString();
     if (!currentText.empty()) {
        currentText += "\n" + message;
    } else {
        currentText = "Game Log:\n" + message;
    }
    std::vector<std::string> lines;
    std::stringstream ss(currentText);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    const int maxLines = 10;
    if (lines.size() > maxLines) {
        lines.erase(lines.begin(), lines.end() - maxLines);
    }
    
    // Reconstruct the text
    std::string newText = "";
    for (size_t i = 0; i < lines.size(); i++) {
        if (i > 0) newText += "\n";
        newText += lines[i];
    }
    
    infoPanelText.setString(newText);
}

void GameGui::updateActionButtonVisibility() {
    Player* currentPlayer = game->get_players()[game->get_turn()];
    updateActionButtonTexts();
    
    for (size_t i = 0; i < availableActions.size(); i++) {
        bool shouldShow = true;
        
        switch (availableActions[i]) {
            case GameAction::GATHER:
                // Hide gather if player is sanctioned
                shouldShow = !currentPlayer->get_isSanction();
                break;
                
            case GameAction::TAX:
                // Hide tax if player is sanctioned
                shouldShow = !currentPlayer->get_isSanction();
                break;
                
            case GameAction::BRIBE:
                // Hide bribe if player doesn't have enough coins
                shouldShow = currentPlayer->get_coins() >= 4;
                break;
                
            case GameAction::ARREST:
                // Hide arrest if player can't arrest 
                shouldShow = currentPlayer->get_canArrest();
                if (shouldShow) {
                    // Check if there are any valid arrest targets
                    std::vector<Player*>& players = game->get_players();
                    bool hasValidTarget = false;
                    
                    for (int j = 0; j < numPlayers; j++) {
                        if (j != game->get_turn() && players[j]->get_isActive()) {
                            if (isValidArrestTarget(players[j])) {
                                hasValidTarget = true;
                                break;
                            }
                        }
                    }
                    
                    shouldShow = hasValidTarget;
                }
                break;
                
            case GameAction::SANCTION:{
                // Hide sanction if player doesn't have enough coins
                std::vector<Player*>& players = game->get_players();
                bool hasValidTarget = false;
                
                for (int j = 0; j < numPlayers; j++) {
                    if (j != game->get_turn() && players[j]->get_isActive()) {
                        if (isValidSanctionTarget(players[j])) {
                            hasValidTarget = true;
                            break;
                        }
                    }
                }
                
                shouldShow = hasValidTarget;
                break;
            }  
            case GameAction::COUP:
                // Hide coup if player doesn't have enough coins
                shouldShow = currentPlayer->get_coins() >= 7;
                break;
                
            case GameAction::UNIQE:
                // Only Baron can invest
                shouldShow = (dynamic_cast<Baron*>(currentPlayer) != nullptr  && currentPlayer->get_coins() >= 3)
                || (dynamic_cast<Spy*>(currentPlayer) != nullptr);
                break;
                
                
            default:
                shouldShow = true;
                break;
        }
        
        if (shouldShow) {
            actionButtons[i].setFillColor(buttonColor);
            actionButtonTexts[i].setFillColor(textColor);
        } else {
            actionButtons[i].setFillColor(sf::Color(50, 50, 50, 100)); // Grayed out
            actionButtonTexts[i].setFillColor(sf::Color(100, 100, 100)); // Gray text
        }
    }
}

void GameGui::initializeVictoryScreen() {
    // Victory overlay (semi-transparent background)
    victoryOverlay.setSize(sf::Vector2f(800, 600));
    victoryOverlay.setPosition(200, 100);
    victoryOverlay.setFillColor(victoryOverlayColor);
    
    // Victory title
    victoryTitleText.setString("GAME OVER");
    if (fontLoaded) victoryTitleText.setFont(font);
    victoryTitleText.setCharacterSize(48);
    victoryTitleText.setFillColor(victoryTextColor);
    victoryTitleText.setStyle(sf::Text::Bold);
    
    // Winner text
    if (fontLoaded) winnerText.setFont(font);
    winnerText.setCharacterSize(36);
    winnerText.setFillColor(sf::Color::White);
    winnerText.setStyle(sf::Text::Bold);
    
    // Winner subtitle
    winnerSubText.setString("Congratulations on your victory!");
    if (fontLoaded) winnerSubText.setFont(font);
    winnerSubText.setCharacterSize(20);
    winnerSubText.setFillColor(sf::Color(200, 200, 200));
    
    // Reset button
    resetButton.setSize(sf::Vector2f(200, 50));
    resetButton.setPosition(500, 450);
    resetButton.setFillColor(resetButtonColor);
    
    resetButtonText.setString("Play Again");
    if (fontLoaded) resetButtonText.setFont(font);
    resetButtonText.setCharacterSize(20);
    resetButtonText.setFillColor(sf::Color::White);
    resetButtonText.setStyle(sf::Text::Bold);
}

void GameGui::checkForWinner() {
    try {
        std::string winner = game->winner();
        showVictoryScreen(winner);
    } catch (const std::runtime_error& e) {
        // No winner yet or multiple players active - continue game
    }
}

void GameGui::showVictoryScreen(const std::string& winner) {
    gameEnded = true;
    winnerName = winner;
    
    // Update winner text
    winnerText.setString(winner + " WINS!");
    
    // Center all text elements
    sf::FloatRect titleBounds = victoryTitleText.getLocalBounds();
    victoryTitleText.setPosition(600 - titleBounds.width / 2, 250);
    
    sf::FloatRect winnerBounds = winnerText.getLocalBounds();
    winnerText.setPosition(600 - winnerBounds.width / 2, 320);
    
    sf::FloatRect subBounds = winnerSubText.getLocalBounds();
    winnerSubText.setPosition(600 - subBounds.width / 2, 380);
    
    // Center reset button text
    sf::FloatRect resetBounds = resetButtonText.getLocalBounds();
    resetButtonText.setPosition(
        500 + 100 - resetBounds.width / 2,
        450 + 25 - resetBounds.height / 2
    );
    
    updateInfoPanel("GAME OVER! " + winner + " is the winner!");
}

void GameGui::drawVictoryScreen() {
    // Draw semi-transparent overlay
    window.draw(victoryOverlay);
    
    // Draw victory text elements
    window.draw(victoryTitleText);
    window.draw(winnerText);
    window.draw(winnerSubText);
    
    // Draw reset button
    window.draw(resetButton);
    window.draw(resetButtonText);
}

void GameGui::resetGame() {
    gameEnded = false;
    winnerName = "";
    gamePhase = 0;
    targetPlayer = -1;
    //isBribe = false;
    waitingForBlock = false;
    blockingPlayer = -1;
    lastActionTarget = -1;
    eligibleBlockers.clear();
    revealedPlayers.clear();
    
    // Clear existing buttons
    targetButtons.clear();
    targetButtonTexts.clear();
    
    // Reset game state
    game->clear_players();
    game->set_turn(0);
    
    // Reinitialize players
    initializePlayers();
    updatePlayerDisplay();
    updateCurrentPlayerDisplay();
    updateActionButtonVisibility();
    
    phaseText.setString("Phase: Action Selection");
    instructionText.setString("Choose an action:");
    updateInfoPanel("New Game Started! Player 1's turn.");
}

bool GameGui::isPointInResetButton(sf::Vector2i point) {
    sf::FloatRect bounds = resetButton.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(point));
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

     // Draw current blocker highlight (only during blocking phase)
    if (gamePhase == 2 && !eligibleBlockers.empty() && 
        currentBlockerIndex < static_cast<int>(eligibleBlockers.size())) {
        window.draw(currentBlockerHighlight);
    }
    
    // Draw action buttons
    if (gamePhase == 0 && !gameEnded) {
        for (size_t i = 0; i < actionButtons.size(); i++) {
            window.draw(actionButtons[i]);
            window.draw(actionButtonTexts[i]);
        }
    }

    // Draw target selection buttons
    if (gamePhase == 1 && !gameEnded) {
        for (size_t i = 0; i < targetButtons.size(); i++) {
            window.draw(targetButtons[i]);
            window.draw(targetButtonTexts[i]);
        }
    }

    // Draw current blocker prompt (only during blocking phase)
    if (gamePhase == 2 && !currentBlockerPrompt.getString().isEmpty() && !gameEnded) {
        window.draw(currentBlockerPrompt);
    }

    // Draw info panel
    window.draw(infoPanel);
    window.draw(infoPanelText);
    
   if (gamePhase == 2 && !gameEnded) {
        window.draw(blockButton);
        window.draw(blockButtonText);
        window.draw(allowButton);
        window.draw(allowButtonText);
    }

    // Always draw reset button 
    window.draw(alwaysResetButton);
    window.draw(alwaysResetText);

    
    // Draw victory screen if game has ended
    if (gameEnded) {
        drawVictoryScreen();
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