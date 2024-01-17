#include "gui/Game.h"
#include "gui/GameMenu.h"

int main()
{
    try {
        sf::RenderWindow window(sf::VideoMode(800, 800), "Chess Game");
        window.setVerticalSyncEnabled(true);

        while (window.isOpen()) {
            chessAi::GameMenu menu(&window);
            auto playerSelection = menu.runMenu();

            chessAi::Game game(&window, std::get<0>(playerSelection), std::get<1>(playerSelection),
                               std::get<2>(playerSelection));
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