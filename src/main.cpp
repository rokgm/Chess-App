#include "gui/Game.h"

#include "spdlog/spdlog.h"

int main()
{
    // Set global log level. Otherwise in debug mode trace and debug will not log.
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));

    chessAi::Game game(800, 800);
    game.runGame();

    return 0;
}