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
  - Responsive interactive side panel
  - Configurable panel width, padding, and display auto-fit behavior
  - Held-key movement input for responsive SFML demos/games
  - Layered wall, floor, marker, and player shading

## Renderer base settings

`SfmlRenderSettings` is the reusable surface for games that want to start from
this renderer and then add their own visual systems. It now includes preferred
values plus bounds for tile size, tile spacing, window padding, and side-panel
width. It also includes `autoFitToDisplay` and `displaySafetyMargin` so large
mazes can stay inside the current interface only when the preferred window would
otherwise exceed the display.

Keyboard feel is controlled by `movementStepSeconds`, while
`playerVisualFollowSpeed` controls how sharply the displayed marker catches up
to the authoritative grid position. This keeps the reusable `Player` model
simple while letting SFML demos feel immediate and smooth.

The right-side panel is drawn from the same responsive layout used for mouse
hit-testing. Labels and status text fit themselves to the available panel width,
which keeps derived projects from needing hard-coded title or option text sizes
as their window and maze dimensions change.

## Cleanup policy

The project keeps `SfmlRendererSmoke` as the one SFML smoke executable because it
uses the reusable renderer directly. Older standalone rendering experiments and
Xcode `xcuserdata` files should stay out of source control so the base project
does not carry duplicated drawing code or user-local IDE state.

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
