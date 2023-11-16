#include "Game.h"
#include "Board.h"

namespace chessAi
{

Game::Game(unsigned int windowWidth, unsigned int windowHeight)
    : m_windowSize(windowWidth, windowHeight),
      m_window(sf::RenderWindow(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Chess Game"))
{
    m_window.setVerticalSyncEnabled(true);
}

// void Game::handleWindowResize()
// {
// }

void Game::runGame()
{
    // sf::RenderWindow window(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Chess Game");
    // window.setVerticalSyncEnabled(true);

    while (m_window.isOpen()) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                m_window.close();
            if (event.type == sf::Event::Resized) {
                m_window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                                        static_cast<float>(event.size.height))));
                m_windowSize.x = event.size.width;
                m_windowSize.y = event.size.height;
            }
        }

        // TODO: move this outside of the loop if possible.
        Board board(m_windowSize.getSmallestAxis() * 4 / 5);

        m_window.clear(sf::Color::Color(128, 128, 128, 255));
        auto boardSprite = board.getBoardSprite();
        boardSprite.setPosition(0.5f * static_cast<sf::Vector2f>(m_windowSize));
        m_window.draw(boardSprite);
        m_window.display();
    }
}

} // namespace chessAi