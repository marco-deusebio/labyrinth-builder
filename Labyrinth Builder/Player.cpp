//
//  Player.cpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/22/26.
//

#include "Player.hpp"

Player::Player(int startRow, int startColumn)
    : row(startRow), column(startColumn)
{
}

int Player::getRow() const
{
    return row;
}

int Player::getColumn() const
{
    return column;
}

void Player::move(int rowChange, int columnChange)
{
    row += rowChange;
    column += columnChange;
}
