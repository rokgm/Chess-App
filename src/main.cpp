#include "gui/Game.h"

int main()
{
    chessAi::Logger::Init();

    chessAi::Game game(800, 800);
    game.runGame();

    return 0;
}