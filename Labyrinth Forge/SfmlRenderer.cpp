//
//  SfmlRenderer.cpp
//  Labyrinth Forge
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#include "SfmlRenderer.hpp"

#include "Maze.hpp"
#include "Player.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace {
const sf::Color backgroundColor(12, 10, 10);
const sf::Color floorColor(18, 16, 16);
const sf::Color wallColor(38, 35, 34);
const sf::Color wallShadowColor(4, 3, 3);
const sf::Color copperOutlineColor(116, 70, 38);
const sf::Color startColor(216, 146, 45);
const sf::Color exitColor(150, 35, 35);
const sf::Color playerColor(42, 204, 220);
const sf::Color playerOutlineColor(170, 245, 255);
const sf::Color playerShadowColor(3, 3, 4);
const sf::Color panelColor(15, 13, 13);
const sf::Color panelBorderColor(82, 50, 30);
const sf::Color panelSlotColor(25, 22, 20);
const sf::Color panelAccentColor(130, 82, 42);
const sf::Color panelTextColor(224, 205, 178);
const sf::Color panelMutedTextColor(160, 132, 105);

constexpr float minimumTileSize = 24.f;
constexpr float maximumTileSize = 72.f;
constexpr float minimumTileSpacing = 0.f;
constexpr float maximumTileSpacing = 12.f;
constexpr float outerMarkerInsetRatio = 0.21f;
constexpr float innerMarkerScale = 0.45f;
constexpr float playerMarkerRadiusRatio = 0.33f;
constexpr float playerShadowOffsetRatio = 0.06f;
constexpr float wallShadowOffsetRatio = 0.08f;
constexpr float mazePanelGap = 28.f;
constexpr float panelSlotLeftInset = 18.f;
constexpr float panelSlotTop = 64.f;
constexpr float panelSlotHeight = 42.f;
constexpr float panelSlotStep = 56.f;
constexpr float panelButtonSize = 26.f;
constexpr float panelButtonGap = 8.f;
constexpr unsigned int minimumWindowWidth = 920;
constexpr unsigned int minimumWindowHeight = 632;

enum class PanelAction {
    none,
    decreaseSpacing,
    increaseSpacing,
    decreaseTileSize,
    increaseTileSize,
    resetView
};

float getTilePitch(const SfmlRenderSettings& settings)
{
    return settings.tileSize + settings.tileSpacing;
}

float getMazePixelWidth(
    const Maze& maze,
    const SfmlRenderSettings& settings
)
{
    return static_cast<float>(maze.getWidth()) * settings.tileSize +
           static_cast<float>(maze.getWidth() - 1) * settings.tileSpacing;
}

float getMazePixelHeight(
    const Maze& maze,
    const SfmlRenderSettings& settings
)
{
    return static_cast<float>(maze.getHeight()) * settings.tileSize +
           static_cast<float>(maze.getHeight() - 1) * settings.tileSpacing;
}

float getPanelLeft(
    const Maze& maze,
    const SfmlRenderSettings& settings
)
{
    return settings.windowPadding +
           getMazePixelWidth(maze, settings) +
           mazePanelGap;
}

sf::FloatRect getPanelSlotBounds(
    float panelLeft,
    float panelTop,
    std::size_t slotIndex,
    const SfmlRenderSettings& settings
)
{
    return {
        {
            panelLeft + panelSlotLeftInset,
            panelTop + panelSlotTop +
                static_cast<float>(slotIndex) * panelSlotStep
        },
        {
            settings.sidePanelWidth - panelSlotLeftInset * 2.f,
            panelSlotHeight
        }
    };
}

sf::FloatRect getPanelButtonBounds(
    float panelLeft,
    float panelTop,
    std::size_t slotIndex,
    std::size_t buttonIndex,
    const SfmlRenderSettings& settings
)
{
    const float rightInset = panelSlotLeftInset;
    const float rightEdge = panelLeft + settings.sidePanelWidth - rightInset;
    const float buttonLeft =
        rightEdge -
        (static_cast<float>(2 - buttonIndex) * panelButtonSize) -
        (static_cast<float>(1 - buttonIndex) * panelButtonGap);
    
    return {
        {
            buttonLeft,
            panelTop + panelSlotTop + 8.f +
                static_cast<float>(slotIndex) * panelSlotStep
        },
        {panelButtonSize, panelButtonSize}
    };
}

PanelAction getPanelActionAt(
    sf::Vector2f mousePosition,
    float panelLeft,
    float panelTop,
    const SfmlRenderSettings& settings
)
{
    if (getPanelButtonBounds(panelLeft, panelTop, 1, 0, settings)
            .contains(mousePosition)) {
        return PanelAction::decreaseSpacing;
    }
    
    if (getPanelButtonBounds(panelLeft, panelTop, 1, 1, settings)
            .contains(mousePosition)) {
        return PanelAction::increaseSpacing;
    }
    
    if (getPanelButtonBounds(panelLeft, panelTop, 2, 0, settings)
            .contains(mousePosition)) {
        return PanelAction::decreaseTileSize;
    }
    
    if (getPanelButtonBounds(panelLeft, panelTop, 2, 1, settings)
            .contains(mousePosition)) {
        return PanelAction::increaseTileSize;
    }
    
    if (getPanelSlotBounds(panelLeft, panelTop, 3, settings)
            .contains(mousePosition)) {
        return PanelAction::resetView;
    }
    
    return PanelAction::none;
}

void preloadInterfaceFontGlyphs(sf::Font& font)
{
    // Preload glyphs at the sizes we use in the UI to avoid lazy rasterization
    // during interactive rendering, which can cause QoS inversions.
    const std::array<unsigned int, 4> sizes{15u, 16u, 18u, 22u};

    // Characters used across labels, buttons, titles, and status lines.
    // Include uppercase/lowercase letters, digits, spaces, and punctuation
    // that appear in the UI and window titles.
    const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        " -+[]:/,.px"; // include common punctuation and units

    for (unsigned int size : sizes) {
        // Ensure the internal texture for this size is created
        (void)font.getTexture(size);

        for (unsigned char c : chars) {
            (void)font.getGlyph(static_cast<char32_t>(c), size, false, 0.f);
        }
    }
}

sf::Color getMarkerColor(char cell)
{
    if (cell == 'S') {
        return startColor;
    }
    
    if (cell == 'E') {
        return exitColor;
    }
    
    return floorColor;
}

sf::Vector2f getTilePosition(
    int row,
    int column,
    float mazeLeft,
    float mazeTop,
    const SfmlRenderSettings& settings
)
{
    const float tilePitch = getTilePitch(settings);
    float x = mazeLeft + static_cast<float>(column) * tilePitch;
    float y = mazeTop + static_cast<float>(row) * tilePitch;
    
    return {x, y};
}

sf::Vector2f getPlayerPosition(
    const Player& player,
    const sf::CircleShape& playerMarker,
    float mazeLeft,
    float mazeTop,
    const SfmlRenderSettings& settings
)
{
    const float tilePitch = getTilePitch(settings);
    
    return {
        mazeLeft +
        static_cast<float>(player.getColumn()) * tilePitch +
        settings.tileSize / 2.f -
        playerMarker.getRadius(),
        
        mazeTop +
        static_cast<float>(player.getRow()) * tilePitch +
        settings.tileSize / 2.f -
        playerMarker.getRadius()
    };
}

void tryMovePlayer(
    const Maze& maze,
    Player& player,
    int rowChange,
    int columnChange
)
{
    int nextRow = player.getRow() + rowChange;
    int nextColumn = player.getColumn() + columnChange;
    
    if (maze.isWalkable(nextRow, nextColumn)) {
        player.move(rowChange, columnChange);
    }
}

void drawFloor(
    sf::RenderWindow& window,
    sf::RectangleShape& floorTile,
    sf::Vector2f tilePosition
)
{
    floorTile.setPosition(tilePosition);
    window.draw(floorTile);
}

void drawWall(
    sf::RenderWindow& window,
    sf::RectangleShape& wallShadowTile,
    sf::RectangleShape& wallTile,
    sf::Vector2f tilePosition,
    const SfmlRenderSettings& settings
)
{
    const float wallShadowOffset =
        std::max(1.f, settings.tileSize * wallShadowOffsetRatio);
    
    wallShadowTile.setPosition({
        tilePosition.x + wallShadowOffset,
        tilePosition.y + wallShadowOffset
    });
    window.draw(wallShadowTile);
    
    wallTile.setPosition(tilePosition);
    window.draw(wallTile);
}

void drawMarker(
    sf::RenderWindow& window,
    char cell,
    sf::CircleShape& outerMarker,
    sf::CircleShape& innerMarker,
    sf::Vector2f tilePosition,
    const SfmlRenderSettings& settings
)
{
    const float outerMarkerInset =
        settings.tileSize * outerMarkerInsetRatio;
    
    outerMarker.setFillColor(getMarkerColor(cell));
    outerMarker.setPosition({
        tilePosition.x + outerMarkerInset,
        tilePosition.y + outerMarkerInset
    });
    window.draw(outerMarker);
    
    innerMarker.setPosition({
        tilePosition.x + settings.tileSize / 2.f - innerMarker.getRadius(),
        tilePosition.y + settings.tileSize / 2.f - innerMarker.getRadius()
    });
    window.draw(innerMarker);
}

void drawPlayer(
    sf::RenderWindow& window,
    const Player& player,
    sf::CircleShape& playerShadow,
    sf::CircleShape& playerMarker,
    float mazeLeft,
    float mazeTop,
    const SfmlRenderSettings& settings
)
{
    const float playerShadowOffset =
        std::max(1.f, settings.tileSize * playerShadowOffsetRatio);
    
    sf::Vector2f playerPosition = getPlayerPosition(
        player,
        playerMarker,
        mazeLeft,
        mazeTop,
        settings
    );
    
    playerShadow.setPosition({
        playerPosition.x + playerShadowOffset,
        playerPosition.y + playerShadowOffset
    });
    window.draw(playerShadow);
    
    playerMarker.setPosition(playerPosition);
    window.draw(playerMarker);
}

void drawMaze(
    sf::RenderWindow& window,
    const Maze& maze,
    sf::RectangleShape& floorTile,
    sf::CircleShape& outerMarker,
    sf::CircleShape& innerMarker,
    sf::RectangleShape& wallShadowTile,
    sf::RectangleShape& wallTile,
    float mazeLeft,
    float mazeTop,
    const SfmlRenderSettings& settings
)
{
    for (int row = 0; row < maze.getHeight(); ++row) {
        for (int column = 0; column < maze.getWidth(); ++column) {
            char cell = maze.getCell(row, column);
            
            sf::Vector2f tilePosition = getTilePosition(
                row,
                column,
                mazeLeft,
                mazeTop,
                settings
            );

            drawFloor(window, floorTile, tilePosition);
            
            if (cell == 'X') {
                drawWall(
                    window,
                    wallShadowTile,
                    wallTile,
                    tilePosition,
                    settings
                );
            } else if (cell == 'S' || cell == 'E') {
                drawMarker(
                    window,
                    cell,
                    outerMarker,
                    innerMarker,
                    tilePosition,
                    settings
                );
            }
        }
    }
}

void drawInterfacePanel(
    sf::RenderWindow& window,
    float panelLeft,
    float panelTop,
    float panelHeight,
    const SfmlRenderSettings& settings,
    const sf::Font* font
)
{
    sf::RectangleShape panel({
        settings.sidePanelWidth,
        panelHeight
    });
    
    panel.setPosition({panelLeft, panelTop});
    panel.setFillColor(panelColor);
    panel.setOutlineColor(panelBorderColor);
    panel.setOutlineThickness(2.f);
    window.draw(panel);
    
    sf::RectangleShape titleBar({
        settings.sidePanelWidth - 36.f,
        12.f
    });
    
    titleBar.setPosition({panelLeft + 18.f, panelTop + 24.f});
    titleBar.setFillColor(panelAccentColor);
    window.draw(titleBar);
    
    const std::array<std::string, 4> labels{
        "Move: Arrow Keys",
        "Spacing: - / +",
        "Tile Size: [ / ]",
        "Reset View: R"
    };
    
    for (std::size_t index = 0; index < labels.size(); ++index) {
        sf::FloatRect slotBounds = getPanelSlotBounds(
            panelLeft,
            panelTop,
            index,
            settings
        );
        
        sf::RectangleShape menuSlot(slotBounds.size);
        
        menuSlot.setPosition(slotBounds.position);
        menuSlot.setFillColor(panelSlotColor);
        menuSlot.setOutlineColor(panelBorderColor);
        menuSlot.setOutlineThickness(1.f);
        window.draw(menuSlot);
        
        sf::RectangleShape accent({
            8.f,
            24.f
        });
        
        accent.setPosition({
            panelLeft + 32.f,
            panelTop + 73.f + static_cast<float>(index) * 56.f
        });
        accent.setFillColor(panelAccentColor);
        window.draw(accent);
        
        if (font != nullptr) {
            sf::Text label(*font, labels[index], 16);
            
            label.setFillColor(panelTextColor);
            label.setPosition({
                panelLeft + 50.f,
                panelTop + 76.f + static_cast<float>(index) * 56.f
            });
            window.draw(label);
        }
        
        if (index == 1 || index == 2) {
            for (std::size_t buttonIndex = 0; buttonIndex < 2; ++buttonIndex) {
                sf::FloatRect buttonBounds = getPanelButtonBounds(
                    panelLeft,
                    panelTop,
                    index,
                    buttonIndex,
                    settings
                );
                sf::RectangleShape button(buttonBounds.size);
                
                button.setPosition(buttonBounds.position);
                button.setFillColor(backgroundColor);
                button.setOutlineColor(panelAccentColor);
                button.setOutlineThickness(1.f);
                window.draw(button);
                
                if (font != nullptr) {
                    sf::Text buttonLabel(
                        *font,
                        buttonIndex == 0 ? "-" : "+",
                        18
                    );
                    
                    buttonLabel.setFillColor(panelTextColor);
                    buttonLabel.setPosition({
                        buttonBounds.position.x + 8.f,
                        buttonBounds.position.y + 1.f
                    });
                    window.draw(buttonLabel);
                }
            }
        }
    }
    
    if (font == nullptr) {
        return;
    }
    
    sf::Text title(*font, "LABYRINTH FORGE", 22);
    title.setFillColor(panelTextColor);
    title.setPosition({panelLeft + 18.f, panelTop + 42.f});
    window.draw(title);
    
    std::vector<std::string> statusLines{
        "Tile: " + std::to_string(static_cast<int>(settings.tileSize)) +
            " px",
        "Spacing: " +
            std::to_string(static_cast<int>(settings.tileSpacing)) + " px",
        "Esc: Quit"
    };
    
    const float statusTop = panelTop + panelHeight - 112.f;
    
    for (std::size_t index = 0; index < statusLines.size(); ++index) {
        sf::Text status(*font, statusLines[index], 15);
        
        status.setFillColor(panelMutedTextColor);
        status.setPosition({
            panelLeft + 18.f,
            statusTop + static_cast<float>(index) * 24.f
        });
        window.draw(status);
    }
}
}

SfmlRenderer::SfmlRenderer()
    : SfmlRenderer(SfmlRenderSettings{})
{
}

SfmlRenderer::SfmlRenderer(SfmlRenderSettings initialSettings)
    : window(
          sf::VideoMode({minimumWindowWidth, minimumWindowHeight}),
          "Labyrinth Forge"
      ),
      settings(initialSettings)
{
    window.setVerticalSyncEnabled(true);
    loadInterfaceFont();
    updateWindowTitle();
}

bool SfmlRenderer::isOpen() const
{
    return window.isOpen();
}

void SfmlRenderer::loadInterfaceFont()
{
    const std::array<std::string, 4> fontPaths{
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Avenir Next.ttc",
        "/System/Library/Fonts/Helvetica.ttc",
        "/Library/Fonts/Arial.ttf"
    };
    
    for (const std::string& fontPath : fontPaths) {
        if (interfaceFont.openFromFile(fontPath)) {
            interfaceFontLoaded = true;
            preloadInterfaceFontGlyphs(interfaceFont);
            return;
        }
    }
    
    interfaceFontLoaded = false;
}

void SfmlRenderer::processEvents()
{
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* keyPressed =
                   event->getIf<sf::Event::KeyPressed>()) {
            processKeyPress(*keyPressed);
        }
    }
}

void SfmlRenderer::processEvents(
    const Maze& maze,
    Player& player
)
{
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* keyPressed =
                   event->getIf<sf::Event::KeyPressed>()) {
            processKeyPress(*keyPressed, maze, player);
        } else if (const auto* mouseButton =
                   event->getIf<sf::Event::MouseButtonPressed>()) {
            processMouseClick(*mouseButton, maze);
        }
    }
}

void SfmlRenderer::processKeyPress(
    const sf::Event::KeyPressed& keyPressed
)
{
    if (keyPressed.code == sf::Keyboard::Key::Escape) {
        window.close();
    } else if (
        keyPressed.code == sf::Keyboard::Key::Equal ||
        keyPressed.code == sf::Keyboard::Key::Add
    ) {
        settings.tileSpacing = std::min(
            maximumTileSpacing,
            settings.tileSpacing + 1.f
        );
        updateWindowTitle();
    } else if (
        keyPressed.code == sf::Keyboard::Key::Hyphen ||
        keyPressed.code == sf::Keyboard::Key::Subtract
    ) {
        settings.tileSpacing = std::max(
            minimumTileSpacing,
            settings.tileSpacing - 1.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::RBracket) {
        settings.tileSize = std::min(
            maximumTileSize,
            settings.tileSize + 2.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::LBracket) {
        settings.tileSize = std::max(
            minimumTileSize,
            settings.tileSize - 2.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::R) {
        settings.tileSize = SfmlRenderSettings{}.tileSize;
        settings.tileSpacing = SfmlRenderSettings{}.tileSpacing;
        updateWindowTitle();
    }
}

void SfmlRenderer::processKeyPress(
    const sf::Event::KeyPressed& keyPressed,
    const Maze& maze,
    Player& player
)
{
    processKeyPress(keyPressed);
    
    if (keyPressed.code == sf::Keyboard::Key::Up) {
        tryMovePlayer(maze, player, -1, 0);
    } else if (keyPressed.code == sf::Keyboard::Key::Down) {
        tryMovePlayer(maze, player, 1, 0);
    } else if (keyPressed.code == sf::Keyboard::Key::Left) {
        tryMovePlayer(maze, player, 0, -1);
    } else if (keyPressed.code == sf::Keyboard::Key::Right) {
        tryMovePlayer(maze, player, 0, 1);
    }
    
    if (maze.isExit(player.getRow(), player.getColumn())) {
        window.setTitle("Labyrinth Forge - Exit Reached");
    }
}

void SfmlRenderer::processMouseClick(
    const sf::Event::MouseButtonPressed& mouseButton,
    const Maze& maze
)
{
    if (mouseButton.button != sf::Mouse::Button::Left) {
        return;
    }
    
    const float panelLeft = getPanelLeft(maze, settings);
    const float panelTop = settings.windowPadding;
    const sf::Vector2f mousePosition{
        static_cast<float>(mouseButton.position.x),
        static_cast<float>(mouseButton.position.y)
    };
    
    switch (getPanelActionAt(mousePosition, panelLeft, panelTop, settings)) {
        case PanelAction::decreaseSpacing:
            settings.tileSpacing = std::max(
                minimumTileSpacing,
                settings.tileSpacing - 1.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::increaseSpacing:
            settings.tileSpacing = std::min(
                maximumTileSpacing,
                settings.tileSpacing + 1.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::decreaseTileSize:
            settings.tileSize = std::max(
                minimumTileSize,
                settings.tileSize - 2.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::increaseTileSize:
            settings.tileSize = std::min(
                maximumTileSize,
                settings.tileSize + 2.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::resetView:
            settings.tileSize = SfmlRenderSettings{}.tileSize;
            settings.tileSpacing = SfmlRenderSettings{}.tileSpacing;
            updateWindowTitle();
            break;
            
        case PanelAction::none:
            break;
    }
}

void SfmlRenderer::updateWindowSize(const Maze& maze)
{
    const float windowWidth =
        settings.windowPadding +
        getMazePixelWidth(maze, settings) +
        mazePanelGap +
        settings.sidePanelWidth +
        settings.windowPadding;
    
    const float windowHeight =
        settings.windowPadding +
        getMazePixelHeight(maze, settings) +
        settings.windowPadding;
    
    sf::Vector2u nextWindowSize{
        std::max(
            minimumWindowWidth,
            static_cast<unsigned int>(windowWidth)
        ),
        std::max(
            minimumWindowHeight,
            static_cast<unsigned int>(windowHeight)
        )
    };
    
    if (window.getSize() != nextWindowSize) {
        window.setSize(nextWindowSize);
    }
    
    window.setView(sf::View(sf::FloatRect{
        {0.f, 0.f},
        {
            static_cast<float>(nextWindowSize.x),
            static_cast<float>(nextWindowSize.y)
        }
    }));
}

void SfmlRenderer::updateWindowTitle()
{
    window.setTitle(
        "Labyrinth Forge - spacing " +
        std::to_string(static_cast<int>(settings.tileSpacing)) +
        " px, tile " +
        std::to_string(static_cast<int>(settings.tileSize)) +
        " px"
    );
}

void SfmlRenderer::render(
    const Maze& maze,
    const Player& player
)
{
    updateWindowSize(maze);
    
    const float mazeLeft = settings.windowPadding;
    const float mazeTop = settings.windowPadding;
    const float mazePixelHeight = getMazePixelHeight(maze, settings);
    const float panelLeft = getPanelLeft(maze, settings);
    
    const float outerMarkerInset =
        settings.tileSize * outerMarkerInsetRatio;
    const float outerMarkerSize =
        settings.tileSize - outerMarkerInset * 2.f;
    const float innerMarkerSize = outerMarkerSize * innerMarkerScale;
    const float playerMarkerRadius =
        settings.tileSize * playerMarkerRadiusRatio;
    
    sf::RectangleShape floorTile({
        settings.tileSize,
        settings.tileSize
    });
    
    floorTile.setFillColor(floorColor);
    floorTile.setOutlineThickness(0.f);
    
    sf::CircleShape outerMarker(outerMarkerSize / 2.f, 4);
    
    outerMarker.setOutlineColor(copperOutlineColor);
    outerMarker.setOutlineThickness(2.f);
    
    sf::CircleShape innerMarker(innerMarkerSize / 2.f, 4);
    
    innerMarker.setFillColor(copperOutlineColor);
    innerMarker.setOutlineThickness(0.f);
    
    sf::RectangleShape wallShadowTile({
        settings.tileSize,
        settings.tileSize
    });
    
    wallShadowTile.setFillColor(wallShadowColor);
    wallShadowTile.setOutlineThickness(0.f);
    
    sf::RectangleShape wallTile({
        settings.tileSize,
        settings.tileSize
    });
    
    wallTile.setFillColor(wallColor);
    wallTile.setOutlineColor(copperOutlineColor);
    wallTile.setOutlineThickness(2.f);
    
    sf::CircleShape playerShadow(playerMarkerRadius);
    
    playerShadow.setFillColor(playerShadowColor);
    playerShadow.setOutlineThickness(0.f);
    
    sf::CircleShape playerMarker(playerMarkerRadius);
    
    playerMarker.setFillColor(playerColor);
    playerMarker.setOutlineColor(playerOutlineColor);
    playerMarker.setOutlineThickness(2.f);
    
    window.clear(backgroundColor);
    
    drawMaze(
        window,
        maze,
        floorTile,
        outerMarker,
        innerMarker,
        wallShadowTile,
        wallTile,
        mazeLeft,
        mazeTop,
        settings
    );

    drawPlayer(
        window,
        player,
        playerShadow,
        playerMarker,
        mazeLeft,
        mazeTop,
        settings
    );
    
    drawInterfacePanel(
        window,
        panelLeft,
        mazeTop,
        mazePixelHeight,
        settings,
        interfaceFontLoaded ? &interfaceFont : nullptr
    );
    
    window.display();
}

