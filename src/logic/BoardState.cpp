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

const std::array<BoardState::Piece, 64>& BoardState::getBoardState() const
{
    return m_boardState;
}

void BoardState::setBoardState(std::array<Piece, 64>& newBoardState)
{
    m_boardState = newBoardState;
}

void BoardState::updateBoardState(std::pair<BoardState::Piece, unsigned int>& selectedPiece,
                                  unsigned int positionOnBoard)
{
    if (m_boardState[selectedPiece.second - 1] != selectedPiece.first) {
        CHESS_LOG_ERROR("Moving piece from square where it doesn't exits.");
    }
    // We substract 1 to the positionOnBoard to get the real position on the board which was
    // compromised in order to distinguish between an empty pair and a non-empty pair which has the
    // position set at 0
    m_boardState[selectedPiece.second - 1] = BoardState::Piece::empty;
    m_boardState[positionOnBoard] = selectedPiece.first;
}

BoardState::BoardState() : m_boardState(getStartingBoardState())
{
}

} // namespace chessAi