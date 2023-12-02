#include "gui/Game.h"

int main()
{
    chessAi::Logger::Init("log/chess-logs.txt");

    chessAi::Game game(800, 800);
    game.runGame();

    return 0;
}