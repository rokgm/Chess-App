#include "Board.h"

#include "spdlog/spdlog.h"

#include <SFML/Graphics.hpp>

namespace chessAi
{

void run()
{
    unsigned int windowSize = 800;
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Chess Game");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    Board board(static_cast<float>(windowSize) / 8);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                                      static_cast<float>(event.size.height))));
            }
        }

        window.clear(sf::Color::Color(128, 128, 128, 255));

        for (const auto& field : board.getBoard()) {
            window.draw(field);
        }

        window.display();
    }
}

} // namespace chessAi

int main()
{
    // Set global log level. Otherwise in debug mode trace and debug will not log.
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));

    chessAi::run();

    return 0;
}