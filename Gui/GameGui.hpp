#ifndef GAMEGUI_HPP
#define GAMEGUI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class GameAction {
    GATHER,
    TAX, 
    BRIBE,
    ARREST,
    SANCTION,
    COUP,     
};

struct PlayerInfo {
    std::string name;
    int coins;
    //int influence; // Number of cards remaining
    bool isActive;
    sf::Vector2f position;
    sf::RectangleShape playerCard;
    sf::Text nameText;
    sf::Text coinsText;
    //sf::Text influenceText;
};

class GameGui {
private:
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;
    
    // Game state
    int numPlayers;
    int currentPlayer;
    int gamePhase; // 0: action selection, 1: target selection, 2: challenge/block
    std::vector<PlayerInfo> players;
    
    // UI Elements
    sf::Text titleText;
    sf::Text currentPlayerText;
    sf::Text phaseText;
    sf::Text instructionText;
    
    // Action buttons
    std::vector<sf::RectangleShape> actionButtons;
    std::vector<sf::Text> actionButtonTexts;
    std::vector<GameAction> availableActions;
    
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
    sf::Color buttonColor;
    sf::Color buttonHoverColor;
    sf::Color textColor;
    
    // Methods
    void initializeColors();
    void initializePlayers();
    void initializeUI();
    void initializeActionButtons();
    void setupPlayerPositions();
    void updatePlayerDisplay();
    void updateCurrentPlayerDisplay();
    void updateActionButtons();
    void handleMouseClick(sf::Vector2i mousePos);
    void handleMouseMove(sf::Vector2i mousePos);
    bool isPointInButton(sf::Vector2i point, const sf::RectangleShape& button);
    void executeAction(GameAction action);
    void nextPlayer();
    void updateInfoPanel(const std::string& message);
    
public:
    GameGui(int playerCount);
    void run();
    void draw();
    void handleEvents();
};

#endif // GAMEGUI_HPP