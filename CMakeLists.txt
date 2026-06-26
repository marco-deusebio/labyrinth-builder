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

add_library(
    LabyrinthForgeCore
    "Labyrinth Forge/Game.cpp"
    "Labyrinth Forge/Maze.cpp"
    "Labyrinth Forge/Player.cpp"
    "Labyrinth Forge/ConsoleRenderer.cpp"
)

target_compile_features(
    LabyrinthForgeCore
    PUBLIC cxx_std_20
)

target_include_directories(
    LabyrinthForgeCore
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Labyrinth Forge"
)

add_library(
    LabyrinthForgeSfmlRenderer
    "Labyrinth Forge/SfmlRenderer.cpp"
)

target_compile_features(
    LabyrinthForgeSfmlRenderer
    PUBLIC cxx_std_20
)

target_link_libraries(
    LabyrinthForgeSfmlRenderer
    PUBLIC LabyrinthForgeCore SFML::Graphics
)

add_executable(
    LabyrinthForge
    "Labyrinth Forge/main.cpp"
)

target_link_libraries(
    LabyrinthForge
    PRIVATE LabyrinthForgeCore
)

add_executable(
    SfmlWindowTest
    "SfmlWindowTest/SfmlWindowTest.cpp"
)

target_compile_features(
    SfmlWindowTest
    PRIVATE cxx_std_20
)

target_link_libraries(
    SfmlWindowTest
    PRIVATE LabyrinthForgeCore SFML::Graphics
)

add_executable(
    SfmlRendererSmoke
    "SfmlRendererSmoke/SfmlRendererSmoke.cpp"
)

target_compile_features(
    SfmlRendererSmoke
    PRIVATE cxx_std_20
)

target_link_libraries(
    SfmlRendererSmoke
    PRIVATE LabyrinthForgeSfmlRenderer
)
