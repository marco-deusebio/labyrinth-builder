//
//  SfmlRenderer.hpp
//  Labyrinth Forge
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
    
    void loadInterfaceFont();
    void processKeyPress(
        const sf::Event::KeyPressed& keyPressed
    );
    void processKeyPress(
        const sf::Event::KeyPressed& keyPressed,
        const Maze& maze,
        Player& player
    );
    void processMouseClick(
        const sf::Event::MouseButtonPressed& mouseButton,
        const Maze& maze
    );
    void updateWindowSize(const Maze& maze);
    void updateWindowTitle();
};
