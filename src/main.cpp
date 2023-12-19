#include "gui/Game.h"

int main()
{
    chessAi::Logger::Init();

    try {
        chessAi::Game game(800, 800);
        game.runGame();
    }
    catch (const std::runtime_error& ex) {
        CHESS_LOG_CRITICAL(ex.what());
    }
    catch (const std::exception& ex) {
        CHESS_LOG_CRITICAL(ex.what());
    }
    catch (...) {
        CHESS_LOG_CRITICAL("Unknown error thrown.");
    }

    return 0;
}