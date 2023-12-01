#pragma once

#include "Board.h"
#include "logic/BoardState.h"

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
    void drawGameTextures();
    void handleWindowResize(const sf::Event& event);

private:
    WindowSize m_windowSize;
    sf::RenderWindow m_window;
    Board m_board;
    std::unordered_map<BoardState::Piece, sf::Texture> m_pieceTextures;
};

} // namespace chessAi