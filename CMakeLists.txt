cmake_minimum_required(VERSION 3.28)

project(LabyrinthForge LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(SFML)

add_executable(
    LabyrinthForge
    "Labyrinth Forge/main.cpp"
    "Labyrinth Forge/Game.cpp"
    "Labyrinth Forge/Maze.cpp"
    "Labyrinth Forge/Player.cpp"
    "Labyrinth Forge/ConsoleRenderer.cpp"
    "Labyrinth Forge/SfmlRenderer.cpp"
)

target_compile_features(
    LabyrinthForge
    PRIVATE cxx_std_20
)

target_link_libraries(
    LabyrinthForge
    PRIVATE SFML::Graphics
)

add_executable(
    SfmlWindowTest
    "SfmlWindowTest/SfmlWindowTest.cpp"
    "Labyrinth Forge/Maze.cpp"
    "Labyrinth Forge/Player.cpp"
)

target_compile_features(
    SfmlWindowTest
    PRIVATE cxx_std_20
)

target_link_libraries(
    SfmlWindowTest
    PRIVATE SFML::Graphics
)

add_executable(
    SfmlRendererSmoke
    "SfmlRendererSmoke/SfmlRendererSmoke.cpp"
    "Labyrinth Forge/Maze.cpp"
    "Labyrinth Forge/Player.cpp"
    "Labyrinth Forge/SfmlRenderer.cpp"
)

target_compile_features(
    SfmlRendererSmoke
    PRIVATE cxx_std_20
)

target_link_libraries(
    SfmlRendererSmoke
    PRIVATE SFML::Graphics
)
