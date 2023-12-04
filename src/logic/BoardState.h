#pragma once

#include <array>
#include <limits>
#include <unordered_map>

namespace chessAi
{

class BoardState
{
public:
    enum class Piece
    {
        whitePawn,
        whiteBishop,
        whiteKnight,
        whiteRook,
        whiteKing,
        whiteQueen,
        blackPawn,
        blackBishop,
        blackKnight,
        blackRook,
        blackKing,
        blackQueen,
        empty
    };

    // TODO: Move this to minimax engine class when it is implemented. Probably doesn't
    // belong here.
    inline const static std::unordered_map<Piece, int> pieceValues = {
        {Piece::whitePawn, 1},    {Piece::whiteBishop, 3},
        {Piece::whiteKnight, 3},  {Piece::whiteRook, 5},
        {Piece::whiteQueen, 9},   {Piece::whiteKing, std::numeric_limits<int>::max()},
        {Piece::blackPawn, -1},   {Piece::blackBishop, -3},
        {Piece::blackKnight, -3}, {Piece::blackRook, -5},
        {Piece::blackQueen, -9},  {Piece::blackKing, std::numeric_limits<int>::min()},
    };

public:
    /**
     * Create a board position of std::array<Piece, 64> representing 8x8 board. First element
     * in array is at upper left on board and they continue to the right of the board an then
     * next row.
     *
     * Starting board position is usual chess starting position.
     */
    BoardState();

    const std::array<Piece, 64>& getBoardState() const;
    void updateBoardState(Piece pieceType, unsigned int oldPosition, unsigned int newPosition);

private:
    std::array<Piece, 64> getStartingBoardState() const;

private:
    std::array<Piece, 64> m_boardState;
};

} // namespace chessAi