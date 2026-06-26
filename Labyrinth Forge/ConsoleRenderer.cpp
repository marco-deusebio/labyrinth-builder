//
//  ConsoleRenderer.cpp
//  Labyrinth Forge
//
//  Created by Marco D’Eusebio on 6/23/26.
//

#include "ConsoleRenderer.hpp"

#include "Maze.hpp"
#include "Player.hpp"

#include <iostream>

void ConsoleRenderer::render(
    const Maze& maze,
    const Player& player
)
{
    for (int row = 0; row < maze.getHeight(); ++row) {
        for (int column = 0; column < maze.getWidth(); ++column) {
            if (row == player.getRow() &&
                column == player.getColumn()) {
                std::cout << '@';
                continue;
            }
            
            char cell = maze.getCell(row, column);
            
            if (cell == 'X') {
                std::cout << "█";
            } else {
                std::cout << cell;
            }
        }
        
        std::cout << '\n';
    }
}
