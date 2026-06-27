//
//  Renderer.hpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/23/26.
//

#pragma once

class Maze;
class Player;

class Renderer {
public:
    virtual ~Renderer() = default;
    
    virtual void render(
        const Maze& maze,
        const Player& player
    ) = 0;
};
