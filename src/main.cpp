#include "gui/Game.h"
#include "gui/GameMenu.h"

std::chrono::milliseconds getTimeLimit(chessAi::Difficulty difficulty)
{
    switch (difficulty) {
    case chessAi::Difficulty::easy:
        return std::chrono::milliseconds(1000);
    case chessAi::Difficulty::medium:
        return std::chrono::milliseconds(3000);
    case chessAi::Difficulty::hard:
        return std::chrono::milliseconds(6000);
    default:
        return std::chrono::milliseconds(1000);
    }
}

int main()
{
    try {
        sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Game");
        window.setFramerateLimit(30);

        while (window.isOpen()) {
            chessAi::GameMenu menu(&window);
            auto playerSelection = menu.runMenu();
            chessAi::Game game(&window, playerSelection.color, playerSelection.againstComputer,
                               getTimeLimit(playerSelection.difficulty), playerSelection.fenString);
            game.runGame();
        }
    }
    catch (const std::exception& ex) {
        CHESS_LOG_CRITICAL(ex.what());
    }
    catch (...) {
        CHESS_LOG_CRITICAL("Unhandled error thrown.");
    }

    return 0;
}