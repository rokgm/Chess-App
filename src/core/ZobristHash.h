#pragma once

#include "PieceBitBoards.h"

namespace chessAi
{

/**
 * https://www.chessprogramming.org/Zobrist_Hashing
 */
class ZobristHash
{
public:
    /**
     * Should only be used when constructing the board, function is slow.
     */
    static uint64_t calculateZobristKey(const PieceBitBoards& boards);

    static const std::array<std::array<uint64_t, 12>, 64>& getPieces();
    static uint64_t getSideToMove();
    static const std::array<uint64_t, 4>& getCastlingRights();
    static const std::array<uint64_t, 8>& getEnPassantFile();

private:
    static void initZobristNumbers();

private:
    /**
     * For each square on board, we generate 12 random numbers for all piece types.
     * Numbers of one square are ordered as in PieceFigure enum. Multiplied by PieceColor.
     * access with get index from piece type
     */
    inline static std::array<std::array<uint64_t, 12>, 64> s_pieces{};
    inline static uint64_t s_sideToMove = 0;
    // Order white king side, white queen side, black king, queen side
    inline static std::array<uint64_t, 4> s_castlingRights{};
    inline static std::array<uint64_t, 8> s_enPassantFile{};
};

} // namespace chessAi