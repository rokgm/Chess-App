#include "ZobristHash.h"

#include <random>

namespace chessAi
{

void ZobristHash::initZobristNumbers()
{
    // We set seed to be always the same.
    std::mt19937_64 rng(29979258);
    std::uniform_int_distribution<uint64_t> distribution(std::numeric_limits<uint64_t>::min(),
                                                         std::numeric_limits<uint64_t>::max());

    for (auto& squaresArray : s_pieces) {
        for (auto& piece : squaresArray) {
            piece = distribution(rng);
        }
    }
    for (auto& right : s_castlingRights) {
        right = distribution(rng);
    }
    for (auto& file : s_enPassantFile) {
        file = distribution(rng);
    }
    s_sideToMove = distribution(rng);
}

uint64_t ZobristHash::calculateZobristKey(const PieceBitBoards& boards)
{
    initZobristNumbers();

    uint64_t key = 0;

    for (uint16_t i = 0; i < 64; ++i) {
        auto type = boards.getPieceTypeWithSetBitAtPosition(i);
        if (type.getPieceFigure() != PieceFigure::Empty) {
            key ^= s_pieces[i][type.getPieceIndex()];
        }
    }

    key ^= s_enPassantFile[boards.enPassantTargetSquare % 8];

    if (boards.currentMoveColor == PieceColor::Black)
        key ^= s_sideToMove;

    if (boards.whiteKingSideCastle)
        key ^= s_castlingRights[0];
    if (boards.whiteQueenSideCastle)
        key ^= s_castlingRights[1];
    if (boards.blackKingSideCastle)
        key ^= s_castlingRights[2];
    if (boards.blackQueenSideCastle)
        key ^= s_castlingRights[3];
    return key;
}

const std::array<std::array<uint64_t, 12>, 64>& ZobristHash::getPieces()
{
    return s_pieces;
}

uint64_t ZobristHash::getSideToMove()
{
    return s_sideToMove;
}

const std::array<uint64_t, 4>& ZobristHash::getCastlingRights()
{
    return s_castlingRights;
}

const std::array<uint64_t, 8>& ZobristHash::getEnPassantFile()
{
    return s_enPassantFile;
}

} // namespace chessAi