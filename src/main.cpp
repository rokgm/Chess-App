#include "Board.h"

#include "spdlog/spdlog.h"

#include <SFML/Graphics.hpp>

namespace chessAi
{

void run()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Game");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    Board board(50);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Color(128, 128, 128, 255));

        for (const auto& row : board.getBoard()) {
            for (const auto& field : row) {
                window.draw(field);
            }
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