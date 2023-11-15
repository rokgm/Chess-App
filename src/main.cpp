#include "spdlog/spdlog.h"

#include <SFML/Graphics.hpp>

int main()
{
#if DEBUG
    // Set global log level. Otherwise in debug build mode trace and debug will not log.
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));
    SPDLOG_TRACE("TEST_TRACE");
    SPDLOG_DEBUG("TEST_DEBUG");
    SPDLOG_INFO("TEST_INFO");
#else if RELEASE
    SPDLOG_WARN("TEST_WARN");
    SPDLOG_ERROR("TEST_ERROR");
    SPDLOG_CRITICAL("TEST_CRITICAL");
#endif

    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}