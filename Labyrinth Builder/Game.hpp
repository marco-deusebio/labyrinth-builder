//
//  Game.hpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/18/26.
//

#pragma once

#include "Renderer.hpp"
#include "Maze.hpp"
#include "Player.hpp"

class Game {
public:
    explicit Game(Renderer& gameRenderer);
    void run();
    void displayTitle();
    void displayMenu();
    int getMenuChoice();

private:
    int mazeWidth;
    int mazeHeight;
    Maze maze;
    Player player;
    Renderer& renderer;
    int moveCount;
    
    void displayLegend() const;
    void playRound();
    void resetRound();
    void changeGenerationSettings();
    void saveDungeon() const;
};
