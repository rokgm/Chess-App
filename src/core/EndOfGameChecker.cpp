#include "EndOfGameChecker.h"
#include "MoveGenerator.h"
#include "PieceBitBoards.h"

namespace chessAi
{

EndOfGameType EndOfGameChecker::checkBoardState(const PieceBitBoards& bitBoards)
{
    bool allEmpty = MoveGeneratorWrapper::generateLegalMoves<MoveType::Normal>(bitBoards).empty();
    bool kingIsInCheck = false;

    if (bitBoards.currentMoveColor == PieceColor::White) {
        kingIsInCheck = MoveGenerator<PieceColor::White>::isKingInCheck(bitBoards);
    }
    else if (bitBoards.currentMoveColor == PieceColor::Black) {
        kingIsInCheck = MoveGenerator<PieceColor::Black>::isKingInCheck(bitBoards);
    }
    else
        CHESS_LOG_ERROR("Piece color is not handled correctly.");

    if (!allEmpty)
        return EndOfGameType::None;
    if (kingIsInCheck)
        return EndOfGameType::Checkmate;
    return EndOfGameType::Stalemate;
}

} // namespace chessAi