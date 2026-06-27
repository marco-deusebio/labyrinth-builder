cmake_minimum_required(VERSION 3.28)

project(LabyrinthBuilder LANGUAGES CXX)

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
    LabyrinthBuilderCore
    "Labyrinth Builder/Game.cpp"
    "Labyrinth Builder/Maze.cpp"
    "Labyrinth Builder/Player.cpp"
    "Labyrinth Builder/ConsoleRenderer.cpp"
)

target_compile_features(
    LabyrinthBuilderCore
    PUBLIC cxx_std_20
)

target_include_directories(
    LabyrinthBuilderCore
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Labyrinth Builder"
)

add_library(
    LabyrinthBuilderSfmlRenderer
    "Labyrinth Builder/SfmlRenderer.cpp"
)

target_compile_features(
    LabyrinthBuilderSfmlRenderer
    PUBLIC cxx_std_20
)

target_link_libraries(
    LabyrinthBuilderSfmlRenderer
    PUBLIC LabyrinthBuilderCore SFML::Graphics
)

add_executable(
    LabyrinthBuilder
    "Labyrinth Builder/main.cpp"
)

target_link_libraries(
    LabyrinthBuilder
    PRIVATE LabyrinthBuilderCore
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
    PRIVATE LabyrinthBuilderCore SFML::Graphics
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
    PRIVATE LabyrinthBuilderSfmlRenderer
)
