#pragma once

#include "Move.h"

#include <memory>
#include <optional>

namespace chessAi
{

struct BookEntry
{
    BookEntry() : move(0, 0, 0, 0), nextMoves() {}
    BookEntry(Move move) : move(move), nextMoves() {}

    Move move;
    std::vector<std::unique_ptr<BookEntry>> nextMoves;
};

/**
 * Get best move from book. Parse csv file to tree structure.
 * CSV file was made from parsing PGN files.
 * Promotions are not handled!
 *
 * TODO: Currently can just follow one line, cannot transpose to another.
 * Maybe refactor to store all possible moves in a postion. Use JSON.
 */
class OpeningBook
{
public:
    /**
     * Parses book.csv.
     *
     * @return true If successful.
     */
    static bool Init();

    /**
     * Get best move for current line. If more are available, random one is chosen.
     *
     * @param movesHistory Previous moves line.
     */
    static std::optional<Move> getBookMove(const std::vector<Move>& movesHistory);

private:
    // Tree structure of moves. First move is null move and store all first moves (next moves).
    inline static std::unique_ptr<BookEntry> s_book = nullptr;
};

} // namespace chessAi