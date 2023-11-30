#include "Game.h"
#include "Board.h"

namespace chessAi
{

Game::Game(unsigned int windowWidth, unsigned int windowHeight)
    : m_windowSize(windowWidth, windowHeight),
      m_window(sf::RenderWindow(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Chess Game")),
      m_board(Board(m_windowSize.getSmallestAxis() * 4 / 5))
{
    m_window.setVerticalSyncEnabled(true);
    m_board.setCenterPosition(m_windowSize);
}

void Game::handleWindowResize(const sf::Event& event)
{
    m_window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                            static_cast<float>(event.size.height))));
    m_windowSize.x = event.size.width;
    m_windowSize.y = event.size.height;
    m_board.resize(m_windowSize.getSmallestAxis() * 4 / 5);
    m_board.setCenterPosition(m_windowSize);
}

void Game::handleEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_window.close();
            break;
        case sf::Event::Resized:
            handleWindowResize(event);
            break;
        default:
            break;
        }
    }
}

void Game::drawGameTextures()
{
    // Set  grey background color.
    m_window.clear(sf::Color(128, 128, 128, 255));

    const auto& boardSprite = m_board.getBoardSprite();

    m_window.draw(boardSprite);
    m_window.display();
}

void Game::runGame()
{
    while (m_window.isOpen()) {
        handleEvents();
        drawGameTextures();
    }
}

} // namespace chessAi