//
//  Player.hpp
//  Labyrinth Forge
//
//  Created by Marco D’Eusebio on 6/22/26.
//

#pragma once

class Player {
public:
    Player(int startRow, int startColumn);
    
    int getRow() const;
    int getColumn() const;
    void move(int rowChange, int columnChange);
    
private:
    int row;
    int column;
};
