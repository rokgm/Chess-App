#pragma once

#include <SFML/Graphics.hpp>

#include <array>
namespace chessAi
{

class Board
{
public:
    /**
     * Construct a new chess board.
     */
    Board(unsigned int boardSize);

    /**
     * Get the board sprite.
     * Sprite origin is set to the middle of the board.
     */
    sf::Sprite getBoardSprite() const;

    void boardResize(unsigned int newSize);

private:
    unsigned int m_boardSize;
    sf::RenderTexture m_boardTexture;
};

} // namespace chessAi