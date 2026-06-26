//
//  Maze.hpp
//  Labyrinth Forge
//
//  Created by Marco D’Eusebio on 6/21/26.
//

#pragma once

#include <string>
#include <vector>

class Maze {
public:
    Maze(int width, int height);
    
    bool isWalkable(int row, int column) const;
    bool isExit(int row, int column) const;
    void saveToFile(const std::string& fileName) const;
    int getWidth() const;
    int getHeight() const;
    char getCell(int row, int column) const;
    
private:
    std::vector<std::vector<char>> grid;
    
    void generate();
    void carvePassagesFrom(int row, int column);
};
