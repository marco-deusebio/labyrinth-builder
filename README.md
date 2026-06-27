# Labyrinth Builder

This is the standalone maze/labyrinth developer project.

It contains the reusable labyrinth systems without the crypt-solver game layer:

- maze generation
- player movement/state
- console rendering
- SFML maze rendering
- tile size and pixel-spacing controls
- auto-fit SFML layout behavior for oversized mazes/windows
- configurable renderer padding and side-panel width
- held-key SFML movement with fast repeat timing and smooth marker follow
- layered SFML wall/floor shading for a more dimensional maze view
- reusable progression helpers

Use this project when you want to build or experiment with the labyrinth engine
itself, or when you want a clean base for a different game idea.

## Reusable SFML Layout Controls

The SFML renderer exposes layout preferences through `SfmlRenderSettings`:

- `tileSize` and `tileSpacing` for maze density
- `windowPadding` for the outer frame
- `sidePanelWidth` for the control/status panel
- minimum and maximum bounds for those values
- `displaySafetyMargin` and `autoFitToDisplay` for responsive window fitting
- `movementStepSeconds` and `playerVisualFollowSpeed` for keyboard feel

The renderer keeps the preferred layout while it fits on the current display. If
the maze plus panel would outgrow the screen, it computes a smaller effective
tile size, tighter padding, and a compact panel for that frame without changing
the caller's preferred settings.

## Build with CMake

```bash
cmake -S . -B build
cmake --build build
```

## Build with Xcode

Open `Labyrinth Builder.xcodeproj` from this folder and build the available app or
test schemes.

## Split-project note

This folder was split from the `codex/labyrinth-base-project` branch of the
original `Labyrinth Forge` repo. It is now a standalone project folder with its
own Git repository and no remote connected to the old combined source repo.
