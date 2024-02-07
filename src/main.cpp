#include "gui/Game.h"
#include "gui/GameMenu.h"

int main()
{
    try {
        sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Game");
        window.setFramerateLimit(30);

        while (window.isOpen()) {
            chessAi::GameMenu menu(&window);
            auto playerSelection = menu.runMenu();

            std::chrono::milliseconds timeLimit(1000);
            switch (std::get<3>(playerSelection)) {
            case chessAi::Difficulty::easy:
                timeLimit = std::chrono::milliseconds(1000);
                break;
            case chessAi::Difficulty::medium:
                timeLimit = std::chrono::milliseconds(3000);
                break;
            case chessAi::Difficulty::hard:
                timeLimit = std::chrono::milliseconds(6000);
                break;
            default:
                break;
            }

            chessAi::Game game(&window, std::get<0>(playerSelection), std::get<1>(playerSelection),
                               timeLimit, std::get<2>(playerSelection));
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