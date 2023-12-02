#pragma once

#include "Board.h"
#include "logic/BoardPosition.h"

#include <SFML/Graphics.hpp>

#include <unordered_map>

namespace chessAi
{

struct WindowSize : public sf::Vector2u
{
    using sf::Vector2u::Vector2;

    unsigned int getSmallestAxis()
    {
        return (x <= y) ? x : y;
    }
};

class Game
{
public:
    /**
     * Construct a new Game object, that will run the gui window and the game logic.
     */
    Game(unsigned int windowWidth, unsigned int windowHeight);

    void runGame();

private:
    void handleEvents();
    void displayGameSprites();
    void handleWindowResize(const sf::Event& event);
    void drawPosition();
    void drawPieceType(BoardPosition::Piece type, int position,
                       const sf::FloatRect& boardGlobalBounds, const float& boardFieldSize);

private:
    WindowSize m_windowSize;
    sf::RenderWindow m_window;
    Board m_board;
    BoardPosition m_boardPosition;
    std::unordered_map<BoardPosition::Piece, sf::Texture> m_pieceTextures;
};

} // namespace chessAi