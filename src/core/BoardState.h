#pragma once

#include "MoveGenerator.h"
#include "PieceBitBoards.h"
#include "PieceType.h"

#include <optional>
namespace chessAi
{

enum class EndOfGameType;

/**
 * Interface to handle the game logic and rules.
 * Rules such as fifty move rule, three fold repetitions etc. are not implemented.
 */
class BoardState
{
public:
    BoardState(const std::string& fenString = "");

    [[nodiscard]] EndOfGameType updateBoardState(Move move);

    void goToPreviousBoardState();

    const PieceBitBoards& getBitBoards() const;

    PieceType getPiece(unsigned int position) const;

    std::optional<Move> getLastMove() const;

    std::vector<uint64_t> getZobristKeyHistory() const;

    const std::vector<Move>& getMovesHistory() const;

    const std::vector<PieceBitBoards>& getBitBoardsHistory() const;

private:
    PieceBitBoards m_bitBoards;
    std::vector<PieceBitBoards> m_bitBoardsHistory;
    std::vector<Move> m_movesHistory;
};

} // namespace chessAi