#pragma once

#include <SFML/Graphics.hpp>

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
    const sf::Sprite& getBoardSprite() const;

    void resize(unsigned int newBoardSize);

    void setCenterPosition(const sf::Vector2u& windowSize);

private:
    unsigned int m_boardSize;
    sf::Sprite m_boardSprite;
    sf::RenderTexture m_boardTexture;
};

} // namespace chessAi