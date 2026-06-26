//
//  SfmlRendererSmoke.cpp
//  Labyrinth Forge
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#include "../Labyrinth Forge/Maze.hpp"
#include "../Labyrinth Forge/Player.hpp"
#include "../Labyrinth Forge/SfmlRenderer.hpp"

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
