//
//  Game.cpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/18/26.
//

#include "Game.hpp"

#include <iostream>
#include <limits>

Game::Game(Renderer& gameRenderer)
    : mazeWidth(15),
      mazeHeight(9),
      maze(mazeWidth, mazeHeight),
      player(1, 1),
      renderer(gameRenderer),
      moveCount(0)
{
}

void Game::run()
{
    displayTitle();
    
    while (true) {
        
        displayMenu();
        
        int menuChoice = getMenuChoice();
        
        if (menuChoice == 1) {
            std::cout << '\n';
            
            resetRound();
            playRound();
            
            std::cout << '\n';
            continue;
        }
        
        if (menuChoice == 2) {
            displayLegend();
            continue;
        }
        
        if (menuChoice == 3) {
            changeGenerationSettings();
            continue;
        }
        
        if (menuChoice == 4) {
            saveDungeon();
            continue;
        }
        
        if (menuChoice == 5) {
            std::cout << "Thanks for playing!\n";
            return;
        }
    }
}

void Game::displayTitle()
{
    std::cout << "  ===============================  \n";
    std::cout << "  | ˜˜˜˜˜ LABYRINTH FORGE ˜˜˜˜˜ |  \n";
    std::cout << "  ===============================  \n";
    std::cout << "A Procedural Dungeon Generator Game\n\n";
}

void Game::displayMenu()
{
    std::cout << "1. Generate Dungeon\n";
    std::cout << "2. View Dungeon Legend\n";
    std::cout << "3. Change Generation Settings\n";
    std::cout << "4. Save Dungeon to File\n";
    std::cout << "5. Exit\n";
}

int Game::getMenuChoice()
{
    int choice = 0;
    
    while (true) {
        std::cout << "-> ";
        
        if (!(std::cin >> choice)) {
            std::cout << "Please enter a number.\n";
            std::cin.clear();
        } else if (choice >= 1 && choice <= 5) {
            std::cin.ignore(
                            std::numeric_limits<std::streamsize>::max(),
                            '\n'
                            );
            
            return choice;
        } else {
            std::cout << "Please choose a number from 1 to 5.\n";
        }
        
        std::cin.ignore(
                        std::numeric_limits<std::streamsize>::max(),
                        '\n'
                        );
    }
}

void Game::displayLegend() const
{
    std::cout << "\nDungeon Legend\n";
    std::cout << "--------------\n";
    std::cout << "█  Wall\n";
    std::cout << "   Open  passage\n";
    std::cout << "S  Entrance\n";
    std::cout << "E  Exit\n";
    std::cout << "@  Player\n\n";
}

void Game::playRound()
{
    renderer.render(maze, player);
    
    while (true) {
        std::cout << "\nMove using the following keys:\n";
        std::cout << "[W] = up,  [A] = left,  "
        << "[S] = down,  [D] = right\n";
        std::cout << "[Q] = quit\n";
        std::cout << "-> ";
        
        char command;
        
        if (!(std::cin >> command)) {
            break;
        }
        
        if (command == 'q' || command == 'Q') {
            break;
        }
        
        int rowChange = 0;
        int columnChange = 0;
        
        switch (command) {
            case 'w':
            case 'W':
                rowChange = -1;
                break;
                
            case 's':
            case 'S':
                rowChange = 1;
                break;
                
            case 'a':
            case 'A':
                columnChange = -1;
                break;
                
            case 'd':
            case 'D':
                columnChange = 1;
                break;
                
            default:
                std::cout << "Unknown command.\n";
                continue;
        }
        
        int nextRow = player.getRow() + rowChange;
        int nextColumn = player.getColumn() + columnChange;
        
        if (maze.isWalkable(nextRow, nextColumn)) {
            player.move(rowChange, columnChange);
            ++moveCount;
        } else {
            std::cout << "A wall blocks your path.\n";
        }
        
        renderer.render(maze, player);
        
        
        if (maze.isExit(player.getRow(), player.getColumn())) {
            std::cout << "\nYou've escaped the labyrinth in "
            << moveCount
            << " moves!\n";
            
            break;
        }
    }
}

void Game::resetRound() {
    maze = Maze(mazeWidth, mazeHeight);
    player = Player(1, 1);
    moveCount = 0;
}

void Game::changeGenerationSettings()
{
    while (true) {
        std::cout << "Enter an odd maze width of at least 5: ";
        
        int newWidth = 0;
        
        if (!(std::cin >> newWidth)) {
            std::cout << "Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );
            continue;
        }
        
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
        
        if (newWidth < 5 || newWidth % 2 == 0) {
            std::cout << "The width must be an odd number of at least 5.\n";
            continue;
        }
        
        mazeWidth = newWidth;
        break;
    }
    
    while (true) {
        std::cout << "Enter an odd maze height of at least 5: ";
        
        int newHeight = 0;
        
        if (!(std::cin >> newHeight)) {
            std::cout << "Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );
            continue;
        }
        
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );
        
        if (newHeight < 5 || newHeight % 2 == 0) {
            std::cout << "The height must be an odd number of at least 5.\n";
            continue;
        }
        
        mazeHeight = newHeight;
        break;
    }
    
    std::cout << "Generation settings updated to "
              << mazeWidth << " x " << mazeHeight << ".\n\n";
}

void Game::saveDungeon() const
{
    std::cout << "Enter a file name or full path: ";
    
    std::string fileName;
    std::getline(std::cin, fileName);
    
    if (fileName.empty()) {
        std::cout << "Save cancelled: no file name was entered.\n\n";
        return;
    }
    
    try {
        maze.saveToFile(fileName);
        std::cout << "Dungeon saved to " << fileName << ".\n\n";
    } catch (const std::exception& error) {
        std::cout << "Unable to save the dungeon: "
                  << error.what() << "\n\n";
    }
}
