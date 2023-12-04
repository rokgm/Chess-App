#pragma once

#include "Board.h"
#include "logic/BoardState.h"

#include <SFML/Graphics.hpp>

#include <optional>
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
    void handleMousePressed(const sf::Event& event);
    bool determineMousePressedOnBoard(int x, int y);
    void movePiece(unsigned int positionX, unsigned int positionY, bool noSelectedPiece);
    void drawPosition();
    void drawPieceType(BoardState::Piece type, unsigned int position,
                       const sf::FloatRect& boardGlobalBounds, float boardFieldSize);

private:
    WindowSize m_windowSize;
    sf::RenderWindow m_window;
    Board m_board;
    BoardState m_boardState;
    std::unordered_map<BoardState::Piece, sf::Texture> m_pieceTextures;
    std::pair<BoardState::Piece, unsigned int> m_selectedPieceOldPosition;
};

} // namespace chessAi