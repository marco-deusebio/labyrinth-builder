//
//  SfmlRenderer.hpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#pragma once

#include "Renderer.hpp"

#include <SFML/Graphics.hpp>

class Maze;
class Player;

struct SfmlRenderSettings {
    float tileSize = 48.f;
    float tileSpacing = 0.f;
    float windowPadding = 72.f;
    float sidePanelWidth = 260.f;
    float minimumTileSize = 18.f;
    float maximumTileSize = 72.f;
    float minimumTileSpacing = 0.f;
    float maximumTileSpacing = 12.f;
    float minimumWindowPadding = 24.f;
    float maximumWindowPadding = 96.f;
    float minimumSidePanelWidth = 220.f;
    float maximumSidePanelWidth = 340.f;
    float displaySafetyMargin = 80.f;
    float movementStepSeconds = 0.075f;
    float playerVisualFollowSpeed = 28.f;
    bool autoFitToDisplay = true;
};

class SfmlRenderer : public Renderer {
public:
    SfmlRenderer();
    explicit SfmlRenderer(SfmlRenderSettings initialSettings);
    
    bool isOpen() const;
    void processEvents();
    void processEvents(
        const Maze& maze,
        Player& player
    );
    
    void render(
        const Maze& maze,
        const Player& player
    ) override;
    void setLabyrinthLevel(int nextLevel);
    
private:
    sf::RenderWindow window;
    SfmlRenderSettings settings;
    sf::Font interfaceFont;
    bool interfaceFontLoaded = false;
    int labyrinthLevel = 1;
    bool movingUp = false;
    bool movingDown = false;
    bool movingLeft = false;
    bool movingRight = false;
    sf::Keyboard::Key activeMovementKey = sf::Keyboard::Key::Unknown;
    sf::Clock movementClock;
    sf::Clock frameClock;
    bool visualPlayerPositionInitialized = false;
    float visualPlayerRow = 0.f;
    float visualPlayerColumn = 0.f;
    
    void loadInterfaceFont();
    void processKeyPress(
        const sf::Event::KeyPressed& keyPressed
    );
    void processKeyPress(
        const sf::Event::KeyPressed& keyPressed,
        const Maze& maze,
        Player& player
    );
    void processKeyRelease(
        const sf::Event::KeyReleased& keyReleased
    );
    void processMouseClick(
        const sf::Event::MouseButtonPressed& mouseButton,
        const Maze& maze
    );
    void updateHeldMovement(
        const Maze& maze,
        Player& player
    );
    void resetMovementInput();
    void updateWindowSize(const Maze& maze);
    void updateWindowTitle();
};
