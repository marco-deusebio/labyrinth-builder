//
//  main.cpp
//  Labyrinth Forge
//
//  Created by Marco D’Eusebio on 6/18/26.
//

#include "ConsoleRenderer.hpp"
#include "Game.hpp"

#include <iostream>

int main()
{
    ConsoleRenderer renderer;
    Game game(renderer);
    
    game.run();
    
    return 0;
}
