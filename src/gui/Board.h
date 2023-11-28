#pragma once

#include <SFML/Graphics.hpp>

namespace chessAi
{

// TODO: Implement in separate file.
struct Pieces
{
    // sf::Texture blackBishop;
    //     sf::Texture texture;
    // if (!texture.loadFromFile("pieces_images/black-bishop.png")) {
    // }
    // texture.setSmooth(true);
    // sf::Sprite sprite(texture);
    // sprite.scale(5, 5);
};

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

    void boardResize(unsigned int newBoardSize);

private:
    Pieces m_pieces;
    unsigned int m_boardSize;
    sf::Sprite m_boardSprite;
    sf::RenderTexture m_boardTexture;
};

} // namespace chessAi