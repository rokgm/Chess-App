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
};

} // namespace chessAi