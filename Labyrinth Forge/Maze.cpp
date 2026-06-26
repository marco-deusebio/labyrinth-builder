//
//  Maze.cpp
//  Labyrinth Forge
//
//  Created by Marco D’Eusebio on 6/21/26.
//

#include "Maze.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <random>
#include <stdexcept>
#include <utility>

Maze::Maze(int width, int height)
{
    bool dimensionsAreValid =
    width >= 5 &&
    height >= 5 &&
    width % 2 != 0 &&
    height % 2 != 0;
    
    if (!dimensionsAreValid) {
        throw std::invalid_argument(
            "Maze dimensions must be odd numbers of at least 5."
        );
    }
    
    grid.assign(height, std::vector<char>(width, 'X'));
    generate();
}


bool Maze::isWalkable(int row, int column) const
{
    bool isInsideMaze =
    row >= 0 &&
    row < static_cast<int>(grid.size()) &&
    column >= 0 &&
    column < static_cast<int>(grid.front().size());
    
    
    if (!isInsideMaze) {
        return false;
    }
    
    return grid[row][column] != 'X';
}

bool Maze::isExit(int row, int column) const
{
    return isWalkable(row, column) && grid[row][column] == 'E';
}

void Maze::generate()
{
    carvePassagesFrom(1, 1);

    grid[1][0] = 'S';
    grid[grid.size() - 2].back() = 'E';
}

void Maze::carvePassagesFrom(int row, int column)
{
    grid[row][column] = ' ';
    
    std::array<std::pair<int, int>, 4> directions = {{
        {-2, 0},
        {2, 0},
        {0, -2},
        {0, 2}
    }};
    
    static std::mt19937 randomEngine(std::random_device{}());
    std::shuffle(directions.begin(), directions.end(), randomEngine);
    
    for (const auto& [rowChange, columnChange] : directions) {
        int nextRow = row + rowChange;
        int nextColumn = column + columnChange;
        
        bool insideMaze =
        nextRow > 0 &&
        nextRow < static_cast<int>(grid.size()) - 1 &&
        nextColumn > 0 &&
        nextColumn < static_cast<int>(grid.front().size()) -  1;
        
        if (!insideMaze) {
            continue;
        }
        
        if (grid[nextRow][nextColumn] != 'X') {
            continue;
        }
        
        int wallRow = row + rowChange / 2;
        int wallColumn = column + columnChange / 2;
        
        grid[wallRow][wallColumn] = ' ';
        carvePassagesFrom(nextRow, nextColumn);
    }
}

void Maze::saveToFile(const std::string& fileName) const {
    std::ofstream outputFile(fileName);
    
    if (!outputFile) {
        throw std::runtime_error("Unable to open the dungeon file.");
    }
    
    for (const auto& row : grid) {
        for (char cell : row) {
            outputFile << cell;
        }
        
        outputFile << '\n';
    }
    
    if (!outputFile) {
        throw std::runtime_error("Unable to finish writing the dungeon file.");
    }
}

int Maze::getWidth() const
{
    return static_cast<int>(grid.front().size());
}

int Maze::getHeight() const
{
    return static_cast<int>(grid.size());
}

char Maze::getCell(int row, int column) const
{
    bool isInsideMaze =
    row >= 0 &&
    row < getHeight() &&
    column >= 0 &&
    column < getWidth();
    
    if(!isInsideMaze) {
        throw std::out_of_range("Maze cell coordinates are out of range.");
    }
    
    return grid[row][column];
}
