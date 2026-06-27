# Labyrinth Builder

This is the standalone maze/labyrinth developer project.

It contains the reusable labyrinth systems without the crypt-solver game layer:

- maze generation
- player movement/state
- console rendering
- SFML maze rendering
- tile size and pixel-spacing controls
- reusable progression helpers

Use this project when you want to build or experiment with the labyrinth engine
itself, or when you want a clean base for a different game idea.

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
