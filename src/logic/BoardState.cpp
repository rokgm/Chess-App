#include "BoardState.h"

namespace chessAi
{

namespace
{

void setStartingState(BoardState::Piece piece, std::array<BoardState::Piece, 64>& board,
                      const std::vector<unsigned int>& positions)
{
    for (auto position : positions) {
        board[position] = piece;
    }
}

} // namespace

std::array<BoardState::Piece, 64> BoardState::getStartingBoardState() const
{
    std::array<Piece, 64> boardState;
    boardState.fill(Piece::empty);
    setStartingState(Piece::blackPawn, boardState, {8, 9, 10, 11, 12, 13, 14, 15});
    setStartingState(Piece::whitePawn, boardState, {55, 54, 53, 52, 51, 50, 49, 48});
    setStartingState(Piece::blackKnight, boardState, {1, 6});
    setStartingState(Piece::whiteKnight, boardState, {62, 57});
    setStartingState(Piece::blackBishop, boardState, {2, 5});
    setStartingState(Piece::whiteBishop, boardState, {61, 58});
    setStartingState(Piece::blackRook, boardState, {0, 7});
    setStartingState(Piece::whiteRook, boardState, {63, 56});
    setStartingState(Piece::blackQueen, boardState, {3});
    setStartingState(Piece::whiteQueen, boardState, {59});
    setStartingState(Piece::blackKing, boardState, {4});
    setStartingState(Piece::whiteKing, boardState, {60});

    return boardState;
}

std::array<BoardState::Piece, 64>& BoardState::getBoardState()
{
    return m_boardState;
}

const std::unordered_map<BoardState::Piece, int>& BoardState::getSelectedPiece() const
{
    return selectedPiece;
}

void BoardState::setSelectedPiece(const BoardState::Piece& piece, int position)
{
    selectedPiece = {{piece, position}};
}

BoardState::BoardState() : m_boardState(getStartingBoardState())
{
}

} // namespace chessAi