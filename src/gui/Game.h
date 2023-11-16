#pragma once

#include <SFML/Graphics.hpp>

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
    void handleWindowResize(const sf::Event& event);

private:
    WindowSize m_windowSize;
    sf::RenderWindow m_window;
};

} // namespace chessAi