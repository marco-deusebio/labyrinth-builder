//
//  SfmlRenderer.cpp
//  Labyrinth Builder
//
//  Created by Marco D’Eusebio on 6/26/26.
//

#include "SfmlRenderer.hpp"

#include "Maze.hpp"
#include "Player.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

namespace {
const sf::Color backgroundColor(12, 10, 10);
const sf::Color floorColor(18, 16, 16);
const sf::Color floorHighlightColor(32, 28, 25);
const sf::Color floorShadeColor(9, 8, 8);
const sf::Color wallColor(38, 35, 34);
const sf::Color wallHighlightColor(74, 64, 56);
const sf::Color wallLowerShadeColor(16, 13, 12);
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

constexpr float outerMarkerInsetRatio = 0.21f;
constexpr float innerMarkerScale = 0.45f;
constexpr float playerMarkerRadiusRatio = 0.33f;
constexpr float playerShadowOffsetRatio = 0.06f;
constexpr float wallShadowOffsetRatio = 0.08f;
constexpr float wallHighlightRatio = 0.14f;
constexpr float wallLowerShadeRatio = 0.18f;
constexpr float floorHighlightRatio = 0.08f;
constexpr float mazePanelGap = 28.f;
constexpr float panelSlotLeftInset = 18.f;
constexpr float panelSlotTop = 64.f;
constexpr float panelSlotHeight = 42.f;
constexpr float panelSlotStep = 56.f;
constexpr float panelButtonSize = 26.f;
constexpr float panelButtonGap = 8.f;
constexpr unsigned int minimumWindowWidth = 920;
constexpr unsigned int minimumWindowHeight = 632;
constexpr float minimumReadableTileSize = 10.f;
constexpr float minimumPanelHeight = 560.f;
constexpr float minimumPanelGap = 16.f;
constexpr float minimumFittedTextSize = 10.f;

struct SfmlRenderLayout {
    SfmlRenderSettings settings;
    sf::Vector2u windowSize;
    float mazeLeft = 0.f;
    float mazeTop = 0.f;
    float mazePixelWidth = 0.f;
    float mazePixelHeight = 0.f;
    float panelLeft = 0.f;
    float panelTop = 0.f;
    float panelHeight = 0.f;
    float panelGap = mazePanelGap;
    bool autoFitApplied = false;
};

struct MovementStep {
    int rowChange = 0;
    int columnChange = 0;
};

enum class PanelAction {
    none,
    decreaseSpacing,
    increaseSpacing,
    decreaseTileSize,
    increaseTileSize,
    decreasePanelWidth,
    increasePanelWidth,
    decreasePadding,
    increasePadding,
    resetView
};

bool isMovementKey(sf::Keyboard::Key key)
{
    return key == sf::Keyboard::Key::Up ||
           key == sf::Keyboard::Key::Down ||
           key == sf::Keyboard::Key::Left ||
           key == sf::Keyboard::Key::Right;
}

MovementStep getMovementStepForKey(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Up) {
        return {-1, 0};
    }
    
    if (key == sf::Keyboard::Key::Down) {
        return {1, 0};
    }
    
    if (key == sf::Keyboard::Key::Left) {
        return {0, -1};
    }
    
    if (key == sf::Keyboard::Key::Right) {
        return {0, 1};
    }
    
    return {};
}

float clampFloat(float value, float minimumValue, float maximumValue)
{
    return std::clamp(value, minimumValue, maximumValue);
}

SfmlRenderSettings getClampedSettings(SfmlRenderSettings settings)
{
    settings.tileSize = clampFloat(
        settings.tileSize,
        settings.minimumTileSize,
        settings.maximumTileSize
    );
    settings.tileSpacing = clampFloat(
        settings.tileSpacing,
        settings.minimumTileSpacing,
        settings.maximumTileSpacing
    );
    settings.windowPadding = clampFloat(
        settings.windowPadding,
        settings.minimumWindowPadding,
        settings.maximumWindowPadding
    );
    settings.sidePanelWidth = clampFloat(
        settings.sidePanelWidth,
        settings.minimumSidePanelWidth,
        settings.maximumSidePanelWidth
    );
    
    return settings;
}

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

sf::Vector2u getMaximumWindowSize(const SfmlRenderSettings& settings)
{
    if (!settings.autoFitToDisplay) {
        return {0u, 0u};
    }
    
    const sf::Vector2u desktopSize = sf::VideoMode::getDesktopMode().size;
    const unsigned int safetyMargin =
        static_cast<unsigned int>(std::max(0.f, settings.displaySafetyMargin));
    
    return {
        desktopSize.x > safetyMargin ? desktopSize.x - safetyMargin : desktopSize.x,
        desktopSize.y > safetyMargin ? desktopSize.y - safetyMargin : desktopSize.y
    };
}

SfmlRenderLayout calculateLayout(
    const Maze& maze,
    const SfmlRenderSettings& preferredSettings
)
{
    SfmlRenderLayout layout;
    layout.settings = getClampedSettings(preferredSettings);
    
    SfmlRenderSettings effectiveSettings = layout.settings;
    const sf::Vector2u maximumWindowSize = getMaximumWindowSize(effectiveSettings);
    const bool hasDisplayLimit =
        maximumWindowSize.x > 0u &&
        maximumWindowSize.y > 0u;
    
    auto calculateDesiredSize = [&maze](
        const SfmlRenderSettings& currentSettings,
        float currentGap
    ) {
        const float mazeWidth = getMazePixelWidth(maze, currentSettings);
        const float mazeHeight = getMazePixelHeight(maze, currentSettings);
        const float desiredWidth =
            currentSettings.windowPadding +
            mazeWidth +
            currentGap +
            currentSettings.sidePanelWidth +
            currentSettings.windowPadding;
        const float desiredHeight =
            currentSettings.windowPadding +
            std::max(mazeHeight, minimumPanelHeight) +
            currentSettings.windowPadding;
        
        return sf::Vector2f{desiredWidth, desiredHeight};
    };
    
    sf::Vector2f desiredSize = calculateDesiredSize(
        effectiveSettings,
        layout.panelGap
    );
    
    if (hasDisplayLimit &&
        (desiredSize.x > static_cast<float>(maximumWindowSize.x) ||
         desiredSize.y > static_cast<float>(maximumWindowSize.y))) {
        layout.autoFitApplied = true;
        
        effectiveSettings.windowPadding = effectiveSettings.minimumWindowPadding;
        effectiveSettings.sidePanelWidth = std::clamp(
            std::min(
                effectiveSettings.sidePanelWidth,
                static_cast<float>(maximumWindowSize.x) * 0.28f
            ),
            effectiveSettings.minimumSidePanelWidth,
            effectiveSettings.maximumSidePanelWidth
        );
        layout.panelGap = minimumPanelGap;
        
        const float availableMazeWidth =
            static_cast<float>(maximumWindowSize.x) -
            effectiveSettings.windowPadding * 2.f -
            layout.panelGap -
            effectiveSettings.sidePanelWidth;
        const float availableMazeHeight =
            static_cast<float>(maximumWindowSize.y) -
            effectiveSettings.windowPadding * 2.f;
        const float widthLimitedTile =
            (availableMazeWidth -
             static_cast<float>(maze.getWidth() - 1) *
                effectiveSettings.tileSpacing) /
            static_cast<float>(maze.getWidth());
        const float heightLimitedTile =
            (availableMazeHeight -
             static_cast<float>(maze.getHeight() - 1) *
                effectiveSettings.tileSpacing) /
            static_cast<float>(maze.getHeight());
        
        effectiveSettings.tileSize = std::min(
            effectiveSettings.tileSize,
            std::min(widthLimitedTile, heightLimitedTile)
        );
        
        if (effectiveSettings.tileSize < effectiveSettings.minimumTileSize &&
            effectiveSettings.tileSpacing > effectiveSettings.minimumTileSpacing) {
            effectiveSettings.tileSpacing = effectiveSettings.minimumTileSpacing;
            
            const float spacingFreeWidthTile =
                availableMazeWidth / static_cast<float>(maze.getWidth());
            const float spacingFreeHeightTile =
                availableMazeHeight / static_cast<float>(maze.getHeight());
            
            effectiveSettings.tileSize = std::min(
                preferredSettings.tileSize,
                std::min(spacingFreeWidthTile, spacingFreeHeightTile)
            );
        }
        
        effectiveSettings.tileSize = std::max(
            minimumReadableTileSize,
            effectiveSettings.tileSize
        );
        desiredSize = calculateDesiredSize(effectiveSettings, layout.panelGap);
    }
    
    layout.settings = effectiveSettings;
    layout.mazePixelWidth = getMazePixelWidth(maze, layout.settings);
    layout.mazePixelHeight = getMazePixelHeight(maze, layout.settings);
    
    const unsigned int desiredWindowWidth = static_cast<unsigned int>(
        std::ceil(desiredSize.x)
    );
    const unsigned int desiredWindowHeight = static_cast<unsigned int>(
        std::ceil(desiredSize.y)
    );
    
    const unsigned int minimumWidth =
        hasDisplayLimit && layout.autoFitApplied ?
            std::min(minimumWindowWidth, maximumWindowSize.x) :
            minimumWindowWidth;
    const unsigned int minimumHeight =
        hasDisplayLimit && layout.autoFitApplied ?
            std::min(minimumWindowHeight, maximumWindowSize.y) :
            minimumWindowHeight;
    
    layout.windowSize = {
        std::max(minimumWidth, desiredWindowWidth),
        std::max(minimumHeight, desiredWindowHeight)
    };
    
    if (hasDisplayLimit && layout.autoFitApplied) {
        layout.windowSize.x = std::min(
            layout.windowSize.x,
            maximumWindowSize.x
        );
        layout.windowSize.y = std::min(
            layout.windowSize.y,
            maximumWindowSize.y
        );
    }
    
    layout.mazeLeft = layout.settings.windowPadding;
    layout.mazeTop = layout.settings.windowPadding;
    layout.panelLeft =
        layout.mazeLeft +
        layout.mazePixelWidth +
        layout.panelGap;
    layout.panelTop = layout.mazeTop;
    layout.panelHeight = std::max(layout.mazePixelHeight, minimumPanelHeight);
    
    return layout;
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
    
    if (getPanelButtonBounds(panelLeft, panelTop, 3, 0, settings)
            .contains(mousePosition)) {
        return PanelAction::decreasePanelWidth;
    }
    
    if (getPanelButtonBounds(panelLeft, panelTop, 3, 1, settings)
            .contains(mousePosition)) {
        return PanelAction::increasePanelWidth;
    }
    
    if (getPanelButtonBounds(panelLeft, panelTop, 4, 0, settings)
            .contains(mousePosition)) {
        return PanelAction::decreasePadding;
    }
    
    if (getPanelButtonBounds(panelLeft, panelTop, 4, 1, settings)
            .contains(mousePosition)) {
        return PanelAction::increasePadding;
    }
    
    if (getPanelSlotBounds(panelLeft, panelTop, 5, settings)
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
    float playerRow,
    float playerColumn,
    const sf::CircleShape& playerMarker,
    float mazeLeft,
    float mazeTop,
    const SfmlRenderSettings& settings
)
{
    const float tilePitch = getTilePitch(settings);
    
    return {
        mazeLeft +
        playerColumn * tilePitch +
        settings.tileSize / 2.f -
        playerMarker.getRadius(),
        
        mazeTop +
        playerRow * tilePitch +
        settings.tileSize / 2.f -
        playerMarker.getRadius()
    };
}

bool tryMovePlayer(
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
        return true;
    }
    
    return false;
}

void drawFloor(
    sf::RenderWindow& window,
    sf::RectangleShape& floorTile,
    sf::RectangleShape& floorHighlight,
    sf::RectangleShape& floorShade,
    sf::Vector2f tilePosition
)
{
    floorTile.setPosition(tilePosition);
    window.draw(floorTile);
    
    floorHighlight.setPosition(tilePosition);
    window.draw(floorHighlight);
    
    floorShade.setPosition({
        tilePosition.x,
        tilePosition.y + floorTile.getSize().y - floorShade.getSize().y
    });
    window.draw(floorShade);
}

void drawWall(
    sf::RenderWindow& window,
    sf::RectangleShape& wallShadowTile,
    sf::RectangleShape& wallTile,
    sf::RectangleShape& wallHighlight,
    sf::RectangleShape& wallLowerShade,
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
    
    wallHighlight.setPosition({
        tilePosition.x,
        tilePosition.y
    });
    window.draw(wallHighlight);
    
    wallLowerShade.setPosition({
        tilePosition.x,
        tilePosition.y + settings.tileSize - wallLowerShade.getSize().y
    });
    window.draw(wallLowerShade);
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
    float playerRow,
    float playerColumn,
    sf::CircleShape& playerShadow,
    sf::CircleShape& playerMarker,
    sf::CircleShape& playerHighlight,
    float mazeLeft,
    float mazeTop,
    const SfmlRenderSettings& settings
)
{
    const float playerShadowOffset =
        std::max(1.f, settings.tileSize * playerShadowOffsetRatio);
    
    sf::Vector2f playerPosition = getPlayerPosition(
        playerRow,
        playerColumn,
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
    
    playerHighlight.setPosition({
        playerPosition.x + playerMarker.getRadius() * 0.5f,
        playerPosition.y + playerMarker.getRadius() * 0.38f
    });
    window.draw(playerHighlight);
}

void drawMaze(
    sf::RenderWindow& window,
    const Maze& maze,
    sf::RectangleShape& floorTile,
    sf::RectangleShape& floorHighlight,
    sf::RectangleShape& floorShade,
    sf::CircleShape& outerMarker,
    sf::CircleShape& innerMarker,
    sf::RectangleShape& wallShadowTile,
    sf::RectangleShape& wallTile,
    sf::RectangleShape& wallHighlight,
    sf::RectangleShape& wallLowerShade,
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

            drawFloor(
                window,
                floorTile,
                floorHighlight,
                floorShade,
                tilePosition
            );
            
            if (cell == 'X') {
                drawWall(
                    window,
                    wallShadowTile,
                    wallTile,
                    wallHighlight,
                    wallLowerShade,
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

unsigned int getFittedCharacterSize(
    const sf::Font& font,
    const std::string& text,
    unsigned int preferredSize,
    float maximumWidth
)
{
    unsigned int fittedSize = preferredSize;
    
    while (fittedSize > static_cast<unsigned int>(minimumFittedTextSize)) {
        sf::Text measuredText(font, text, fittedSize);
        
        if (measuredText.getLocalBounds().size.x <= maximumWidth) {
            return fittedSize;
        }
        
        --fittedSize;
    }
    
    return static_cast<unsigned int>(minimumFittedTextSize);
}

void drawFittedText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    unsigned int preferredSize,
    sf::Vector2f position,
    float maximumWidth,
    sf::Color color
)
{
    sf::Text fittedText(
        font,
        text,
        getFittedCharacterSize(font, text, preferredSize, maximumWidth)
    );
    
    fittedText.setFillColor(color);
    fittedText.setPosition(position);
    window.draw(fittedText);
}

void drawCenteredFittedText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    unsigned int preferredSize,
    const sf::FloatRect& bounds,
    sf::Color color
)
{
    sf::Text fittedText(
        font,
        text,
        getFittedCharacterSize(font, text, preferredSize, bounds.size.x - 8.f)
    );
    const sf::FloatRect localBounds = fittedText.getLocalBounds();
    
    fittedText.setFillColor(color);
    fittedText.setPosition({
        bounds.position.x + (bounds.size.x - localBounds.size.x) / 2.f -
            localBounds.position.x,
        bounds.position.y + (bounds.size.y - localBounds.size.y) / 2.f -
            localBounds.position.y - 1.f
    });
    window.draw(fittedText);
}

void drawInterfacePanel(
    sf::RenderWindow& window,
    float panelLeft,
    float panelTop,
    float panelHeight,
    const SfmlRenderSettings& settings,
    const SfmlRenderSettings& preferredSettings,
    bool autoFitApplied,
    const sf::Font* font,
    int labyrinthLevel,
    const Maze& maze
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
    
    const std::array<std::string, 6> labels{
        "Move: Arrow Keys",
        "Spacing: " +
            std::to_string(static_cast<int>(preferredSettings.tileSpacing)) +
            " px",
        "Tile: " +
            std::to_string(static_cast<int>(preferredSettings.tileSize)) +
            " px",
        "Panel: " +
            std::to_string(static_cast<int>(preferredSettings.sidePanelWidth)) +
            " px",
        "Padding: " +
            std::to_string(static_cast<int>(preferredSettings.windowPadding)) +
            " px",
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
            const float reservedButtonWidth =
                (index >= 1 && index <= 4) ?
                    panelButtonSize * 2.f + panelButtonGap + 14.f :
                    0.f;
            
            drawFittedText(
                window,
                *font,
                labels[index],
                16,
                {
                    panelLeft + 50.f,
                    panelTop + 76.f + static_cast<float>(index) * 56.f
                },
                std::max(
                    48.f,
                    settings.sidePanelWidth -
                        panelSlotLeftInset * 2.f -
                        32.f -
                        reservedButtonWidth
                ),
                panelTextColor
            );
        }
        
        if (index >= 1 && index <= 4) {
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
                    drawCenteredFittedText(
                        window,
                        *font,
                        buttonIndex == 0 ? "-" : "+",
                        18,
                        buttonBounds,
                        panelTextColor
                    );
                }
            }
        }
    }
    
    if (font == nullptr) {
        return;
    }
    
    drawFittedText(
        window,
        *font,
        "LABYRINTH BUILDER",
        22,
        {panelLeft + 18.f, panelTop + 42.f},
        settings.sidePanelWidth - 36.f,
        panelTextColor
    );
    
    std::vector<std::string> statusLines{
        "Labyrinth: " + std::to_string(labyrinthLevel),
        "Size: " + std::to_string(maze.getWidth()) + " x " +
            std::to_string(maze.getHeight()),
        "Rendered tile: " +
            std::to_string(static_cast<int>(std::round(settings.tileSize))) +
            " px",
        autoFitApplied ? "Auto-fit: On" : "Auto-fit: Ready",
        "Esc: Quit"
    };
    
    const float statusTop = panelTop + panelHeight - 136.f;
    
    for (std::size_t index = 0; index < statusLines.size(); ++index) {
        drawFittedText(
            window,
            *font,
            statusLines[index],
            15,
            {
                panelLeft + 18.f,
                statusTop + static_cast<float>(index) * 24.f
            },
            settings.sidePanelWidth - 36.f,
            panelMutedTextColor
        );
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
          "Labyrinth Builder"
      ),
      settings(initialSettings)
{
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    loadInterfaceFont();
    updateWindowTitle();
}

bool SfmlRenderer::isOpen() const
{
    return window.isOpen();
}

void SfmlRenderer::setLabyrinthLevel(int nextLevel)
{
    labyrinthLevel = std::max(1, nextLevel);
    updateWindowTitle();
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
            resetMovementInput();
            window.close();
        } else if (const auto* keyPressed =
                   event->getIf<sf::Event::KeyPressed>()) {
            processKeyPress(*keyPressed);
        } else if (const auto* keyReleased =
                   event->getIf<sf::Event::KeyReleased>()) {
            processKeyRelease(*keyReleased);
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
            resetMovementInput();
            window.close();
        } else if (const auto* keyPressed =
                   event->getIf<sf::Event::KeyPressed>()) {
            processKeyPress(*keyPressed, maze, player);
        } else if (const auto* keyReleased =
                   event->getIf<sf::Event::KeyReleased>()) {
            processKeyRelease(*keyReleased);
        } else if (const auto* mouseButton =
                   event->getIf<sf::Event::MouseButtonPressed>()) {
            processMouseClick(*mouseButton, maze);
        }
    }
    
    updateHeldMovement(maze, player);
}

void SfmlRenderer::processKeyPress(
    const sf::Event::KeyPressed& keyPressed
)
{
    if (keyPressed.code == sf::Keyboard::Key::Escape) {
        resetMovementInput();
        window.close();
    } else if (
        keyPressed.code == sf::Keyboard::Key::Equal ||
        keyPressed.code == sf::Keyboard::Key::Add
    ) {
        settings.tileSpacing = std::min(
            settings.maximumTileSpacing,
            settings.tileSpacing + 1.f
        );
        updateWindowTitle();
    } else if (
        keyPressed.code == sf::Keyboard::Key::Hyphen ||
        keyPressed.code == sf::Keyboard::Key::Subtract
    ) {
        settings.tileSpacing = std::max(
            settings.minimumTileSpacing,
            settings.tileSpacing - 1.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::RBracket) {
        settings.tileSize = std::min(
            settings.maximumTileSize,
            settings.tileSize + 2.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::LBracket) {
        settings.tileSize = std::max(
            settings.minimumTileSize,
            settings.tileSize - 2.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::P) {
        settings.sidePanelWidth = std::min(
            settings.maximumSidePanelWidth,
            settings.sidePanelWidth + 10.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::O) {
        settings.sidePanelWidth = std::max(
            settings.minimumSidePanelWidth,
            settings.sidePanelWidth - 10.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::I) {
        settings.windowPadding = std::min(
            settings.maximumWindowPadding,
            settings.windowPadding + 4.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::U) {
        settings.windowPadding = std::max(
            settings.minimumWindowPadding,
            settings.windowPadding - 4.f
        );
        updateWindowTitle();
    } else if (keyPressed.code == sf::Keyboard::Key::R) {
        settings.tileSize = SfmlRenderSettings{}.tileSize;
        settings.tileSpacing = SfmlRenderSettings{}.tileSpacing;
        settings.windowPadding = SfmlRenderSettings{}.windowPadding;
        settings.sidePanelWidth = SfmlRenderSettings{}.sidePanelWidth;
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
    
    if (!isMovementKey(keyPressed.code)) {
        return;
    }
    
    if (keyPressed.code == sf::Keyboard::Key::Up) {
        movingUp = true;
    } else if (keyPressed.code == sf::Keyboard::Key::Down) {
        movingDown = true;
    } else if (keyPressed.code == sf::Keyboard::Key::Left) {
        movingLeft = true;
    } else if (keyPressed.code == sf::Keyboard::Key::Right) {
        movingRight = true;
    }
    
    activeMovementKey = keyPressed.code;
    
    const MovementStep movementStep = getMovementStepForKey(activeMovementKey);
    (void)tryMovePlayer(
        maze,
        player,
        movementStep.rowChange,
        movementStep.columnChange
    );
    movementClock.restart();
}

void SfmlRenderer::processKeyRelease(
    const sf::Event::KeyReleased& keyReleased
)
{
    if (keyReleased.code == sf::Keyboard::Key::Up) {
        movingUp = false;
    } else if (keyReleased.code == sf::Keyboard::Key::Down) {
        movingDown = false;
    } else if (keyReleased.code == sf::Keyboard::Key::Left) {
        movingLeft = false;
    } else if (keyReleased.code == sf::Keyboard::Key::Right) {
        movingRight = false;
    }
    
    if (activeMovementKey != keyReleased.code) {
        return;
    }
    
    if (movingUp) {
        activeMovementKey = sf::Keyboard::Key::Up;
    } else if (movingDown) {
        activeMovementKey = sf::Keyboard::Key::Down;
    } else if (movingLeft) {
        activeMovementKey = sf::Keyboard::Key::Left;
    } else if (movingRight) {
        activeMovementKey = sf::Keyboard::Key::Right;
    } else {
        activeMovementKey = sf::Keyboard::Key::Unknown;
    }
}

void SfmlRenderer::updateHeldMovement(
    const Maze& maze,
    Player& player
)
{
    if (!isMovementKey(activeMovementKey)) {
        return;
    }
    
    const float movementStepSeconds = std::max(
        0.035f,
        settings.movementStepSeconds
    );
    
    if (movementClock.getElapsedTime().asSeconds() < movementStepSeconds) {
        return;
    }
    
    const MovementStep movementStep = getMovementStepForKey(activeMovementKey);
    (void)tryMovePlayer(
        maze,
        player,
        movementStep.rowChange,
        movementStep.columnChange
    );
    movementClock.restart();
}

void SfmlRenderer::resetMovementInput()
{
    movingUp = false;
    movingDown = false;
    movingLeft = false;
    movingRight = false;
    activeMovementKey = sf::Keyboard::Key::Unknown;
}

void SfmlRenderer::processMouseClick(
    const sf::Event::MouseButtonPressed& mouseButton,
    const Maze& maze
)
{
    if (mouseButton.button != sf::Mouse::Button::Left) {
        return;
    }
    
    const SfmlRenderLayout layout = calculateLayout(maze, settings);
    const float panelLeft = layout.panelLeft;
    const float panelTop = layout.panelTop;
    const sf::Vector2f mousePosition{
        static_cast<float>(mouseButton.position.x),
        static_cast<float>(mouseButton.position.y)
    };
    
    switch (getPanelActionAt(
        mousePosition,
        panelLeft,
        panelTop,
        layout.settings
    )) {
        case PanelAction::decreaseSpacing:
            settings.tileSpacing = std::max(
                settings.minimumTileSpacing,
                settings.tileSpacing - 1.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::increaseSpacing:
            settings.tileSpacing = std::min(
                settings.maximumTileSpacing,
                settings.tileSpacing + 1.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::decreaseTileSize:
            settings.tileSize = std::max(
                settings.minimumTileSize,
                settings.tileSize - 2.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::increaseTileSize:
            settings.tileSize = std::min(
                settings.maximumTileSize,
                settings.tileSize + 2.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::decreasePanelWidth:
            settings.sidePanelWidth = std::max(
                settings.minimumSidePanelWidth,
                settings.sidePanelWidth - 10.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::increasePanelWidth:
            settings.sidePanelWidth = std::min(
                settings.maximumSidePanelWidth,
                settings.sidePanelWidth + 10.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::decreasePadding:
            settings.windowPadding = std::max(
                settings.minimumWindowPadding,
                settings.windowPadding - 4.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::increasePadding:
            settings.windowPadding = std::min(
                settings.maximumWindowPadding,
                settings.windowPadding + 4.f
            );
            updateWindowTitle();
            break;
            
        case PanelAction::resetView:
            settings.tileSize = SfmlRenderSettings{}.tileSize;
            settings.tileSpacing = SfmlRenderSettings{}.tileSpacing;
            settings.windowPadding = SfmlRenderSettings{}.windowPadding;
            settings.sidePanelWidth = SfmlRenderSettings{}.sidePanelWidth;
            updateWindowTitle();
            break;
            
        case PanelAction::none:
            break;
    }
}

void SfmlRenderer::updateWindowSize(const Maze& maze)
{
    const SfmlRenderLayout layout = calculateLayout(maze, settings);
    
    if (window.getSize() != layout.windowSize) {
        window.setSize(layout.windowSize);
    }
    
    window.setView(sf::View(sf::FloatRect{
        {0.f, 0.f},
        {
            static_cast<float>(layout.windowSize.x),
            static_cast<float>(layout.windowSize.y)
        }
    }));
}

void SfmlRenderer::updateWindowTitle()
{
    window.setTitle(
        "Labyrinth Builder Base - Labyrinth " +
        std::to_string(labyrinthLevel) +
        ", spacing " +
        std::to_string(static_cast<int>(settings.tileSpacing)) +
        " px, tile " +
        std::to_string(static_cast<int>(settings.tileSize)) +
        " px, panel " +
        std::to_string(static_cast<int>(settings.sidePanelWidth)) +
        " px"
    );
}

void SfmlRenderer::render(
    const Maze& maze,
    const Player& player
)
{
    updateWindowSize(maze);
    
    const SfmlRenderLayout layout = calculateLayout(maze, settings);
    const SfmlRenderSettings& renderSettings = layout.settings;
    const float mazeLeft = layout.mazeLeft;
    const float mazeTop = layout.mazeTop;
    const float mazePixelHeight = layout.mazePixelHeight;
    const float panelLeft = layout.panelLeft;
    
    const float outerMarkerInset =
        renderSettings.tileSize * outerMarkerInsetRatio;
    const float outerMarkerSize =
        renderSettings.tileSize - outerMarkerInset * 2.f;
    const float innerMarkerSize = outerMarkerSize * innerMarkerScale;
    const float playerMarkerRadius =
        renderSettings.tileSize * playerMarkerRadiusRatio;
    const float floorHighlightHeight = std::max(
        1.f,
        renderSettings.tileSize * floorHighlightRatio
    );
    const float floorShadeHeight = std::max(
        1.f,
        renderSettings.tileSize * floorHighlightRatio
    );
    const float wallHighlightHeight = std::max(
        1.f,
        renderSettings.tileSize * wallHighlightRatio
    );
    const float wallLowerShadeHeight = std::max(
        1.f,
        renderSettings.tileSize * wallLowerShadeRatio
    );
    
    const float frameSeconds = std::min(
        0.05f,
        frameClock.restart().asSeconds()
    );
    
    if (!visualPlayerPositionInitialized) {
        visualPlayerRow = static_cast<float>(player.getRow());
        visualPlayerColumn = static_cast<float>(player.getColumn());
        visualPlayerPositionInitialized = true;
    }
    
    const float targetPlayerRow = static_cast<float>(player.getRow());
    const float targetPlayerColumn = static_cast<float>(player.getColumn());
    const float playerDistance =
        std::abs(targetPlayerRow - visualPlayerRow) +
        std::abs(targetPlayerColumn - visualPlayerColumn);
    
    if (playerDistance > 2.f) {
        visualPlayerRow = targetPlayerRow;
        visualPlayerColumn = targetPlayerColumn;
    } else {
        const float followRatio = 1.f -
            std::exp(-settings.playerVisualFollowSpeed * frameSeconds);
        visualPlayerRow += (targetPlayerRow - visualPlayerRow) * followRatio;
        visualPlayerColumn +=
            (targetPlayerColumn - visualPlayerColumn) * followRatio;
    }
    
    sf::RectangleShape floorTile({
        renderSettings.tileSize,
        renderSettings.tileSize
    });
    
    floorTile.setFillColor(floorColor);
    floorTile.setOutlineThickness(0.f);
    
    sf::RectangleShape floorHighlight({
        renderSettings.tileSize,
        floorHighlightHeight
    });
    
    floorHighlight.setFillColor(floorHighlightColor);
    floorHighlight.setOutlineThickness(0.f);
    
    sf::RectangleShape floorShade({
        renderSettings.tileSize,
        floorShadeHeight
    });
    
    floorShade.setFillColor(floorShadeColor);
    floorShade.setOutlineThickness(0.f);
    
    sf::CircleShape outerMarker(outerMarkerSize / 2.f, 4);
    
    outerMarker.setOutlineColor(copperOutlineColor);
    outerMarker.setOutlineThickness(2.f);
    
    sf::CircleShape innerMarker(innerMarkerSize / 2.f, 4);
    
    innerMarker.setFillColor(copperOutlineColor);
    innerMarker.setOutlineThickness(0.f);
    
    sf::RectangleShape wallShadowTile({
        renderSettings.tileSize,
        renderSettings.tileSize
    });
    
    wallShadowTile.setFillColor(wallShadowColor);
    wallShadowTile.setOutlineThickness(0.f);
    
    sf::RectangleShape wallTile({
        renderSettings.tileSize,
        renderSettings.tileSize
    });
    
    wallTile.setFillColor(wallColor);
    wallTile.setOutlineColor(copperOutlineColor);
    wallTile.setOutlineThickness(2.f);
    
    sf::RectangleShape wallHighlight({
        renderSettings.tileSize,
        wallHighlightHeight
    });
    
    wallHighlight.setFillColor(wallHighlightColor);
    wallHighlight.setOutlineThickness(0.f);
    
    sf::RectangleShape wallLowerShade({
        renderSettings.tileSize,
        wallLowerShadeHeight
    });
    
    wallLowerShade.setFillColor(wallLowerShadeColor);
    wallLowerShade.setOutlineThickness(0.f);
    
    sf::CircleShape playerShadow(playerMarkerRadius);
    
    playerShadow.setFillColor(playerShadowColor);
    playerShadow.setOutlineThickness(0.f);
    
    sf::CircleShape playerMarker(playerMarkerRadius);
    
    playerMarker.setFillColor(playerColor);
    playerMarker.setOutlineColor(playerOutlineColor);
    playerMarker.setOutlineThickness(2.f);
    
    sf::CircleShape playerHighlight(std::max(2.f, playerMarkerRadius * 0.22f));
    
    playerHighlight.setFillColor(sf::Color(205, 255, 255, 185));
    playerHighlight.setOutlineThickness(0.f);
    
    window.clear(backgroundColor);
    
    drawMaze(
        window,
        maze,
        floorTile,
        floorHighlight,
        floorShade,
        outerMarker,
        innerMarker,
        wallShadowTile,
        wallTile,
        wallHighlight,
        wallLowerShade,
        mazeLeft,
        mazeTop,
        renderSettings
    );

    drawPlayer(
        window,
        visualPlayerRow,
        visualPlayerColumn,
        playerShadow,
        playerMarker,
        playerHighlight,
        mazeLeft,
        mazeTop,
        renderSettings
    );
    
    drawInterfacePanel(
        window,
        panelLeft,
        mazeTop,
        std::max(mazePixelHeight, minimumPanelHeight),
        renderSettings,
        settings,
        layout.autoFitApplied,
        interfaceFontLoaded ? &interfaceFont : nullptr,
        labyrinthLevel,
        maze
    );
    
    window.display();
}
