//
//  ConsoleRenderer.hpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/23/26.
//

#pragma once

#include "Renderer.hpp"

class Maze;
class Player;

class ConsoleRenderer : public Renderer {
public:
    void render(
        const Maze& maze,
        const Player& player
    ) override;
};
