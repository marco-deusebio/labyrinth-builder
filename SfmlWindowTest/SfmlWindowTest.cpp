#include <SFML/Graphics.hpp>

#include "../Labyrinth Forge/Maze.hpp"
#include "../Labyrinth Forge/Player.hpp"

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

void tryMovePlayer(Player& player, const Maze& maze, int rowChange, int columnChange)
{
    int nextRow = player.getRow() + rowChange;
    int nextColumn = player.getColumn() + columnChange;
    
    if (maze.isWalkable(nextRow, nextColumn)) {
        player.move(rowChange, columnChange);
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

void drawMarker(
    sf::RenderWindow& window,
    char cell,
    sf::CircleShape& outerMarker,
    sf::CircleShape& innerMarker,
    float tileSize,
    float outerMarkerInset,
    sf::Vector2f tilePosition
)
{
    outerMarker.setFillColor(getMarkerColor(cell));
    outerMarker.setPosition({
        tilePosition.x + outerMarkerInset,
        tilePosition.y + outerMarkerInset
    });
    window.draw(outerMarker);
    
    innerMarker.setPosition({
        tilePosition.x + tileSize / 2.f - innerMarker.getRadius(),
        tilePosition.y + tileSize / 2.f - innerMarker.getRadius()
    });
    window.draw(innerMarker);
}

void drawWall(
    sf::RenderWindow& window,
    sf::RectangleShape& wallShadowTile,
    sf::RectangleShape& wallTile,
    float wallShadowOffset,
    sf::Vector2f tilePosition
)
{
    wallShadowTile.setPosition({
        tilePosition.x + wallShadowOffset,
        tilePosition.y + wallShadowOffset
    });
    window.draw(wallShadowTile);
    
    wallTile.setPosition(tilePosition);
    window.draw(wallTile);
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

sf::Vector2f getTilePosition(
    int row,
    int column,
    float tileSize,
    float mazeLeft,
    float mazeTop
)
{
    float x = mazeLeft + static_cast<float>(column) * tileSize;
    float y = mazeTop + static_cast<float>(row) * tileSize;
    
    return {x, y};
}

void drawMaze(
    sf::RenderWindow& window,
    const Maze& maze,
    sf::RectangleShape& floorTile,
    sf::CircleShape& outerMarker,
    sf::CircleShape& innerMarker,
    sf::RectangleShape& wallShadowTile,
    sf::RectangleShape& wallTile,
    float tileSize,
    float outerMarkerInset,
    float wallShadowOffset,
    float mazeLeft,
    float mazeTop
)
{
    for (int row = 0; row < maze.getHeight(); ++row) {
        for (int column = 0; column < maze.getWidth(); ++column) {
            
            char cell = maze.getCell(row, column);
            
            sf::Vector2f tilePosition = getTilePosition(
                row,
                column,
                tileSize,
                mazeLeft,
                mazeTop
            );
            
            drawFloor(window, floorTile, tilePosition);
            
            if (cell == 'X') {
                drawWall(
                    window,
                    wallShadowTile,
                    wallTile,
                    wallShadowOffset,
                    tilePosition
                );
            } else if (cell == 'S' || cell == 'E') {
                drawMarker(
                    window,
                    cell,
                    outerMarker,
                    innerMarker,
                    tileSize,
                    outerMarkerInset,
                    tilePosition
                );
            }
        }
    }
}

sf::Vector2f getPlayerPosition(
    const Player& player,
    const sf::CircleShape& playerMarker,
    float tileSize,
    float mazeLeft,
    float mazeTop
)
{
    return {
        mazeLeft +
        static_cast<float>(player.getColumn()) * tileSize +
        tileSize / 2.f -
        playerMarker.getRadius(),
        
        mazeTop +
        static_cast<float>(player.getRow()) * tileSize +
        tileSize / 2.f -
        playerMarker.getRadius()
    };
}

void drawPlayer(
    sf::RenderWindow& window,
    const Player& player,
    sf::CircleShape& playerShadow,
    sf::CircleShape& playerMarker,
    float tileSize,
    float playerShadowOffset,
    float mazeLeft,
    float mazeTop
)
{
    sf::Vector2f playerPosition = getPlayerPosition(
        player,
        playerMarker,
        tileSize,
        mazeLeft,
        mazeTop
    );
    
    playerShadow.setPosition({
        playerPosition.x + playerShadowOffset,
        playerPosition.y + playerShadowOffset
    });
    window.draw(playerShadow);
    
    playerMarker.setPosition(playerPosition);
    window.draw(playerMarker);
}

void handleKeyPressed(
    sf::RenderWindow& window,
    const sf::Event::KeyPressed& keyPressed,
    Player& player,
    const Maze& maze
)
{
    if (keyPressed.code == sf::Keyboard::Key::Up) {
        tryMovePlayer(player, maze, -1, 0);
    } else if (keyPressed.code == sf::Keyboard::Key::Down) {
        tryMovePlayer(player, maze, 1, 0);
    } else if (keyPressed.code == sf::Keyboard::Key::Left) {
        tryMovePlayer(player, maze, 0, -1);
    } else if (keyPressed.code == sf::Keyboard::Key::Right) {
        tryMovePlayer(player, maze, 0, 1);
    }
    
    if (maze.isExit(player.getRow(), player.getColumn())) {
        window.setTitle("Labyrinth Forge - Exit Reached!");
    }
}

int main()
{
    constexpr int mazeWidth = 15;
    constexpr int mazeHeight = 9;
    
    constexpr float tileSize = 48.f;
    constexpr float outerMarkerInset = 10.f;
    constexpr float outerMarkerSize = tileSize - outerMarkerInset * 2.f;
    constexpr float innerMarkerScale = 0.45f;
    constexpr float innerMarkerSize = outerMarkerSize * innerMarkerScale;
    constexpr float playerMarkerRadius = 16.f;
    constexpr float playerShadowOffset = 3.f;
    constexpr float wallShadowOffset = 4.f;
    constexpr float windowPadding = 100.f;
    
    constexpr float mazeLeft = windowPadding;
    constexpr float mazeTop = windowPadding;
    constexpr float mazeRightMargin = windowPadding;
    constexpr float mazeBottomMargin = windowPadding;
    
    constexpr unsigned int windowWidth =
    static_cast<unsigned int>(
        mazeLeft +
        static_cast<float>(mazeWidth) * tileSize +
        mazeRightMargin
    );
    
    constexpr unsigned int windowHeight =
    static_cast<unsigned int>(
        mazeTop +
        static_cast<float>(mazeHeight) * tileSize +
        mazeBottomMargin
    );
    
    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        "Labyrinth Forge"
    );

    window.setVerticalSyncEnabled(true);
    
    sf::RectangleShape floorTile({tileSize, tileSize});
    
    floorTile.setFillColor(floorColor);
    floorTile.setOutlineThickness(0.f);
    
    sf::CircleShape outerMarker(outerMarkerSize / 2.f, 4);
    
    outerMarker.setOutlineColor(copperOutlineColor);
    outerMarker.setOutlineThickness(2.f);
    
    sf::CircleShape innerMarker(innerMarkerSize / 2.f, 4);
    
    innerMarker.setFillColor(copperOutlineColor);
    innerMarker.setOutlineThickness(0.f);
    
    sf::RectangleShape wallShadowTile({tileSize, tileSize});
    
    wallShadowTile.setFillColor(wallShadowColor);
    wallShadowTile.setOutlineThickness(0.f);
    
    sf::RectangleShape wallTile({tileSize, tileSize});
    
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
    
    Maze maze(mazeWidth, mazeHeight);
    Player player(1, 0);
    
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                handleKeyPressed(window, *keyPressed, player, maze);
            }
        }

        window.clear(backgroundColor);
        
        drawMaze(
            window,
            maze,
            floorTile,
            outerMarker,
            innerMarker,
            wallShadowTile,
            wallTile,
            tileSize,
            outerMarkerInset,
            wallShadowOffset,
            mazeLeft,
            mazeTop
        );
        drawPlayer(
            window,
            player,
            playerShadow,
            playerMarker,
            tileSize,
            playerShadowOffset,
            mazeLeft,
            mazeTop
        );
        
        window.display();
    }

    return 0;
}
