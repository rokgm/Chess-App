#include "BoardPosition.h"

#include "spdlog/spdlog.h"

#include <vector>

namespace chessAi
{

namespace
{

uint64_t modifyBit(uint64_t number, unsigned int position, uint64_t value)
{
    uint64_t mask = 1ULL << position;
    return ((number & ~mask) | (value << position));
}

uint64_t modifyMultipleBits(uint64_t number, const std::vector<unsigned int>& positions,
                            uint64_t value)
{
    for (auto position : positions) {
        number = modifyBit(number, position, value);
    }
    return number;
}

void setStartingPosition(BoardPosition::Piece piece,
                         std::unordered_map<BoardPosition::Piece, uint64_t>& board,
                         const std::vector<unsigned int>& positions)
{
    uint64_t state = modifyMultipleBits(0, positions, 1);
    SPDLOG_TRACE("Piece::{}", static_cast<int>(piece));
    SPDLOG_TRACE("Position for piece is {0:b}.", state);
    if (!board.insert({piece, std::move(state)}).second) {
        SPDLOG_ERROR("Starting positions for BoardPosition::Piece::{} is not inserted to starting "
                     "board state.",
                     static_cast<int>(piece));
    }
}

} // namespace

std::vector<int> BoardPosition::getIndicesForSetBits(uint64_t num)
{
    std::vector<int> positions;

    for (int i = 0; i < 64; ++i) {
        if ((num >> i) & 1) {
            positions.push_back(i);
        }
    }

    return positions;
}

std::unordered_map<BoardPosition::Piece, uint64_t> BoardPosition::getStartingBoardPosition() const
{
    std::unordered_map<Piece, uint64_t> boardPosition;

    setStartingPosition(Piece::blackPawn, boardPosition, {8, 9, 10, 11, 12, 13, 14, 15});
    setStartingPosition(Piece::whitePawn, boardPosition, {55, 54, 53, 52, 51, 50, 49, 48});
    setStartingPosition(Piece::blackKnight, boardPosition, {1, 6});
    setStartingPosition(Piece::whiteKnight, boardPosition, {62, 57});
    setStartingPosition(Piece::blackBishop, boardPosition, {2, 5});
    setStartingPosition(Piece::whiteBishop, boardPosition, {61, 58});
    setStartingPosition(Piece::blackRook, boardPosition, {0, 7});
    setStartingPosition(Piece::whiteRook, boardPosition, {63, 56});
    setStartingPosition(Piece::blackQueen, boardPosition, {3});
    setStartingPosition(Piece::whiteQueen, boardPosition, {59});
    setStartingPosition(Piece::blackKing, boardPosition, {4});
    setStartingPosition(Piece::whiteKing, boardPosition, {60});

    return boardPosition;
}

const std::unordered_map<BoardPosition::Piece, uint64_t>& BoardPosition::getBoardPosition() const
{
    return m_boardPosition;
}

BoardPosition::BoardPosition() : m_boardPosition(getStartingBoardPosition())
{
}

} // namespace chessAi