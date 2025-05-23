#include "MainGui.hpp"
#include "GameGui.hpp"
#include <iostream>

PlayerSelectionGUI::PlayerSelectionGUI() 
    : window(sf::VideoMode(600, 400), "Coup - Player Selection")
    , selectedPlayers(2)
    , fontLoaded(false)
{
    // Try to load a font (you may need to adjust the path)
    if (!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/Arial.ttf") &&
        !font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cout << "Warning: Could not load font. Using default font." << std::endl;
        fontLoaded = false;
    } else {
        fontLoaded = true;
    }
    
    initializeColors();
    initializeTexts();
    initializeButtons();
}

void PlayerSelectionGUI::initializeColors() {
    backgroundColor = sf::Color(40, 44, 52);     // Dark blue-gray
    buttonColor = sf::Color(80, 100, 120);       // Light blue-gray
    selectedButtonColor = sf::Color(100, 149, 237); // Cornflower blue
    buttonHoverColor = sf::Color(135, 206, 235);  // Sky blue
    textColor = sf::Color::White;
}

void PlayerSelectionGUI::initializeTexts() {
    // Title
    titleText.setString("COUP");
    if (fontLoaded) titleText.setFont(font);
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color(255, 215, 0)); // Gold
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(250, 30);
    
    // Instruction
    instructionText.setString("Select number of players:");
    if (fontLoaded) instructionText.setFont(font);
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(textColor);
    instructionText.setPosition(180, 100);
    
    // Player count display
    playerCountText.setString("Players: 2");
    if (fontLoaded) playerCountText.setFont(font);
    playerCountText.setCharacterSize(18);
    playerCountText.setFillColor(textColor);
    playerCountText.setPosition(250, 320);
}

void PlayerSelectionGUI::initializeButtons() {
    // Player selection buttons (2-6)
    for (int i = 0; i < 5; i++) {
        playerButtons[i].setSize(sf::Vector2f(80, 50));
        playerButtons[i].setPosition(50 + i * 100, 150);
        playerButtons[i].setFillColor(buttonColor);
        
        buttonTexts[i].setString(std::to_string(i + 2));
        if (fontLoaded) buttonTexts[i].setFont(font);
        buttonTexts[i].setCharacterSize(24);
        buttonTexts[i].setFillColor(textColor);
        buttonTexts[i].setStyle(sf::Text::Bold);
        
        // Center text in button
        sf::FloatRect textBounds = buttonTexts[i].getLocalBounds();
        buttonTexts[i].setPosition(
            50 + i * 100 + 40 - textBounds.width / 2,
            150 + 25 - textBounds.height / 2
        );
    }
    
    // Set first button as selected (2 players)
    playerButtons[0].setFillColor(selectedButtonColor);
    
    // Start button
    startButton.setSize(sf::Vector2f(150, 50));
    startButton.setPosition(225, 250);
    startButton.setFillColor(sf::Color(34, 139, 34)); // Forest green
    
    startButtonText.setString("START GAME");
    if (fontLoaded) startButtonText.setFont(font);
    startButtonText.setCharacterSize(16);
    startButtonText.setFillColor(textColor);
    startButtonText.setStyle(sf::Text::Bold);
    
    sf::FloatRect startTextBounds = startButtonText.getLocalBounds();
    startButtonText.setPosition(
        225 + 75 - startTextBounds.width / 2,
        250 + 25 - startTextBounds.height / 2
    );
}

bool PlayerSelectionGUI::isPointInButton(sf::Vector2i point, const sf::RectangleShape& button) {
    sf::FloatRect bounds = button.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(point));
}

void PlayerSelectionGUI::handleMouseClick(sf::Vector2i mousePos) {
    // Check player selection buttons
    for (int i = 0; i < 5; i++) {
        if (isPointInButton(mousePos, playerButtons[i])) {
            // Reset all buttons to normal color
            for (int j = 0; j < 5; j++) {
                playerButtons[j].setFillColor(buttonColor);
            }
            
            // Set selected button
            playerButtons[i].setFillColor(selectedButtonColor);
            selectedPlayers = i + 2;
            
            // Update player count text
            playerCountText.setString("Players: " + std::to_string(selectedPlayers));
            return;
        }
    }
    
    // Check start button
    if (isPointInButton(mousePos, startButton)) {
        window.close();
        // Launch the main game with selected number of players
        GameGui gameGui(selectedPlayers);
        gameGui.run();
    }
}

void PlayerSelectionGUI::handleMouseMove(sf::Vector2i mousePos) {
    // Handle hover effects for player buttons
    for (int i = 0; i < 5; i++) {
        if (isPointInButton(mousePos, playerButtons[i])) {
            if (selectedPlayers != i + 2) { // Don't change color if it's selected
                playerButtons[i].setFillColor(buttonHoverColor);
            }
        } else {
            if (selectedPlayers != i + 2) { // Don't change color if it's selected
                playerButtons[i].setFillColor(buttonColor);
            }
        }
    }
    
    // Handle hover effect for start button
    if (isPointInButton(mousePos, startButton)) {
        startButton.setFillColor(sf::Color(50, 205, 50)); // Lime green
    } else {
        startButton.setFillColor(sf::Color(34, 139, 34)); // Forest green
    }
}

void PlayerSelectionGUI::draw() {
    window.clear(backgroundColor);
    
    // Draw all UI elements
    window.draw(titleText);
    window.draw(instructionText);
    window.draw(playerCountText);
    
    // Draw player buttons
    for (int i = 0; i < 5; i++) {
        window.draw(playerButtons[i]);
        window.draw(buttonTexts[i]);
    }
    
    // Draw start button
    window.draw(startButton);
    window.draw(startButtonText);
    
    window.display();
}

void PlayerSelectionGUI::handleEvents() {
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

int PlayerSelectionGUI::run() {
    while (window.isOpen()) {
        handleEvents();
        draw();
    }
    
    return selectedPlayers;
}


int player_selection() {
    PlayerSelectionGUI gui;
    return gui.run();
}