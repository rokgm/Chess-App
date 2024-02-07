#pragma once

namespace chessAi
{

enum class EndOfGameType
{
    None,
    Checkmate,
    Stalemate
};

struct PieceBitBoards;

class EndOfGameChecker
{
public:
    /**
     * @param historyZobristKeys Used for detecting 3 fold repetition.
     *
     *
     * Checks current board state and returns end of game state if stalemate or checkmate.
     */
    static EndOfGameType checkBoardState(const PieceBitBoards& bitBoards);
};

} // namespace chessAi