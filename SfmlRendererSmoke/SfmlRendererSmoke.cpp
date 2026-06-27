//
//  SfmlRendererSmoke.cpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#include "../Labyrinth Builder/Maze.hpp"
#include "../Labyrinth Builder/Player.hpp"
#include "../Labyrinth Builder/SfmlRenderer.hpp"

int main()
{
    Maze maze(15, 9);
    Player player(1, 0);
    SfmlRenderer renderer;
    
    while (renderer.isOpen()) {
        renderer.processEvents();
        renderer.render(maze, player);
    }
    
    return 0;
}
