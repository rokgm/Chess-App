#pragma once

#include <array>
#include <limits>
#include <unordered_map>

namespace chessAi
{

class BoardPosition
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
        blackQueen
    };

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
     * Create a board position of 64 bit numbers (8x8) representing position for
     * each type of piece. 1 for piece, 0 for empty. First digit from the right represents
     * (0,0) position on board (upper left).\n
     * (0, 0) ... (0, 7) \n
     *   .           .   \n
     *   .           .   \n
     *   .           .   \n
     * (7, 0) ... (7, 7) \n
     *
     * Starting board position is usual chess starting position.
     */
    BoardPosition();

    const std::unordered_map<Piece, uint64_t>& getBoardPosition() const;

    static std::vector<int> findSetBits(uint64_t num);

private:
    std::unordered_map<Piece, uint64_t> getStartingBoardPosition() const;

private:
    std::unordered_map<Piece, uint64_t> m_boardPosition;
};

} // namespace chessAi