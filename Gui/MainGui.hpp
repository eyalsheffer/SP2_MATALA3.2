#ifndef MAINGUI_HPP
#define MAINGUI_HPP

#include <SFML/Graphics.hpp>

class PlayerSelectionGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Text titleText;
    sf::Text instructionText;
    sf::Text playerCountText;
    sf::RectangleShape playerButtons[5]; // For 2-6 players
    sf::Text buttonTexts[5];
    sf::RectangleShape startButton;
    sf::Text startButtonText;
    
    int selectedPlayers;
    bool fontLoaded;
    
    // Colors
    sf::Color backgroundColor;
    sf::Color buttonColor;
    sf::Color selectedButtonColor;
    sf::Color buttonHoverColor;
    sf::Color textColor;
    
    void initializeColors();
    void initializeTexts();
    void initializeButtons();
    void handleMouseClick(sf::Vector2i mousePos);
    void handleMouseMove(sf::Vector2i mousePos);
    bool isPointInButton(sf::Vector2i point, const sf::RectangleShape& button);
    
public:
    PlayerSelectionGUI();
    int run(); // Returns selected number of players, or -1 if window closed
    void draw();
    void handleEvents();
};

// Standalone function for backward compatibility
int player_selection();

#endif 