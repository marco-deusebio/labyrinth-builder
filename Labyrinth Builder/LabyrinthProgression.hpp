//
//  LabyrinthProgression.hpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#pragma once

#include "Maze.hpp"
#include "Player.hpp"

#include <algorithm>

namespace labyrinth {

struct LabyrinthProgressionSettings {
    int baseWidth = 15;
    int baseHeight = 9;
    int maximumWidth = 31;
    int maximumHeight = 19;
    int sizeIncreasePerLevel = 2;
    int playerStartRow = 1;
    int playerStartColumn = 0;
};

inline int getDimensionForLevel(
    int baseDimension,
    int maximumDimension,
    int sizeIncreasePerLevel,
    int level
)
{
    return std::min(
        maximumDimension,
        baseDimension + (std::max(1, level) - 1) * sizeIncreasePerLevel
    );
}

inline int getMazeWidthForLevel(
    int level,
    const LabyrinthProgressionSettings& settings
)
{
    return getDimensionForLevel(
        settings.baseWidth,
        settings.maximumWidth,
        settings.sizeIncreasePerLevel,
        level
    );
}

inline int getMazeHeightForLevel(
    int level,
    const LabyrinthProgressionSettings& settings
)
{
    return getDimensionForLevel(
        settings.baseHeight,
        settings.maximumHeight,
        settings.sizeIncreasePerLevel,
        level
    );
}

inline Maze createMazeForLevel(
    int level,
    const LabyrinthProgressionSettings& settings
)
{
    return Maze(
        getMazeWidthForLevel(level, settings),
        getMazeHeightForLevel(level, settings)
    );
}

inline Player createPlayerAtEntrance(
    const LabyrinthProgressionSettings& settings
)
{
    return Player(settings.playerStartRow, settings.playerStartColumn);
}

}
