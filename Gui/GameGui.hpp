#ifndef GAMEGUI_HPP
#define GAMEGUI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>
#include "../Game.hpp"

enum class GameAction {
    GATHER,
    TAX, 
    BRIBE,
    ARREST,
    SANCTION,
    COUP,
    INVEST,
    REVEAL,     
};

struct PlayerGui {

    sf::Vector2f position;
    sf::RectangleShape playerCard;
    sf::Text nameText;
    sf::Text coinsText;
    sf::Text roleText;
    sf::Text statusText;

};

class GameGui {
private:
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;
    int numPlayers;
    int gamePhase; // 0: action selection, 1: target selection, 2: block/allow
    std::vector<PlayerGui> playersGui;
    GameAction pendingAction;
    int targetPlayer;
    Game* game;
    std::mt19937 rng;
    bool waitingForBlock;
    int blockingPlayer;
    GameAction lastAction;
    int lastActionTarget;
    std::vector<std::string> roleNames;
    bool isBribe;
    std::vector<int> eligibleBlockers;      // Indices of players who can block
    int currentBlockerIndex;                // Current blocker being asked
    std::string currentBlockerName;         // Name of current blocker for display
    
    // UI Elements
    sf::Text titleText;
    sf::Text currentPlayerText;
    sf::Text phaseText;
    sf::Text instructionText;
    sf::Text roleText;
    sf::Text statusText;
    
    // Action buttons
    std::vector<sf::RectangleShape> actionButtons;
    std::vector<sf::Text> actionButtonTexts;
    std::vector<GameAction> availableActions;
    std::vector<sf::RectangleShape> targetButtons;
    std::vector<sf::Text> targetButtonTexts;
    
    // Game info panel
    sf::RectangleShape infoPanel;
    sf::Text infoPanelText;

    
    // Challenge/Block buttons
    //sf::RectangleShape challengeButton;
    sf::RectangleShape blockButton;
    sf::RectangleShape allowButton;
    //sf::Text challengeButtonText;
    sf::Text blockButtonText;
    sf::Text allowButtonText;
    
    // Colors
    sf::Color backgroundColor;
    sf::Color panelColor;
    sf::Color activePlayerColor;
    sf::Color inactivePlayerColor;
    sf::Color coupedPlayerColor;
    sf::Color buttonColor;
    sf::Color buttonHoverColor;
    sf::Color textColor;
    
    // Methods
    void initializeColors();
    void initializePlayers();
    std::string getRoleName(Player* player);
    std::string getPlayerStatus(Player* player);
    void initializeUI();
    void initializeActionButtons();
    void setupPlayerPositions();
    void updatePlayerDisplay();
    void updateCurrentPlayerDisplay();
    void updateActionButtons();
    void handleMouseClick(sf::Vector2i mousePos);
    void handleMouseMove(sf::Vector2i mousePos);
    void executeTargetedAction(int targetIndex);
    bool hasGeneralToBlock();
    bool hasGovernorToBlock();
    bool hasJudgeToBlock();
    void startBlockingSequence();
    void handleBlock();
    void handleAllow();
    bool isPointInButton(sf::Vector2i point, const sf::RectangleShape& button);
    void executeAction(GameAction action);
    void setupTargetSelection();
    void nextPlayer();
    void updateInfoPanel(const std::string& message);
  
    void showCurrentBlockerOption();
    void executeAllowedAction();
    
public:
    GameGui(int playerCount);
    void run();
    void draw();
    void handleEvents();
};

#endif // GAMEGUI_HPP