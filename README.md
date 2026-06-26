# Labyrinth Forge

[![C++ Standard](https://shields.io)](https://cppreference.com)
[![License: MIT](https://shields.io)](https://opensource.org)

A professional C++ / Objective-C application codebase managed via structural parent workspaces.

## Getting Started

### Prerequisites
* macOS with **Xcode Developer Tools** installed or a standard C++ compiler with CMake.
* A compliant C++ compiler (Clang/GCC).
* Objective-C / Objective-C++ support when building `.m` or `.mm` source files.

### Compilation & Build Execution

#### Via Xcode
Open the nested Xcode files directly inside Xcode and press `Command-R` to compile and execute the main project application target pipeline.

#### Via CMake
To build this project using the generated CMake setup:
```bash
mkdir build && cd build
cmake ..
cmake --build .
./labyrinth-forge
```

## Source Architecture Layout
Source files are organized inside the active working tree parent directory layouts.

## Codebase Reference Documentation
Code documentation structure is maintained with Doxygen and, when available, Graphviz relationship maps.
