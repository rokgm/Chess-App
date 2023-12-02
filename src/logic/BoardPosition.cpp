#include "BoardPosition.h"

namespace chessAi
{

namespace
{

void setStartingPosition(BoardPosition::Piece piece, std::array<BoardPosition::Piece, 64>& board,
                         const std::vector<unsigned int>& positions)
{
    for (auto position : positions) {
        board[position] = piece;
    }
}

} // namespace

std::array<BoardPosition::Piece, 64> BoardPosition::getStartingBoardPosition() const
{
    std::array<Piece, 64> boardPosition;
    boardPosition.fill(Piece::empty);
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

const std::array<BoardPosition::Piece, 64>& BoardPosition::getBoardPosition() const
{
    return m_boardPosition;
}

BoardPosition::BoardPosition() : m_boardPosition(getStartingBoardPosition())
{
}

} // namespace chessAi