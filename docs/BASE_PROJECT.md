# Labyrinth Builder Base Project

This branch separates the reusable labyrinth-building layer from game-specific
experiments.

## Reusable targets

- `LabyrinthBuilderCore`
  - Maze generation
  - Player position/state
  - Console renderer
  - Console game loop
  - Progression helpers in `LabyrinthProgression.hpp`

- `LabyrinthBuilderSfmlRenderer`
  - SFML maze renderer
  - Pixel spacing controls
  - Tile-size controls
  - Basic interactive side panel

## Game-specific branches

Branches such as a crypt-solver game should reuse `LabyrinthBuilderCore` for maze
generation and player progression, then add their own game systems on top:

- puzzle rules
- long-running save data
- unlockables
- upgrades
- custom UI and art direction
- additional victory conditions

Keeping those systems outside the core makes the maze developer layer reusable
for future games without dragging one game's mechanics into every project.
