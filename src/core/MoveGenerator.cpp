#include "MoveGenerator.h"

namespace chessAi
{

std::vector<Move> MoveGeneratorWrapper::generateLegalMoves(const PieceBitBoards& bitBoards)
{
    std::vector<Move> moves;

    if (bitBoards.currentMoveColor == PieceColor::White) {

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::White, PieceFigure::Pawn>())) {
            for (auto move :
                 MoveGenerator<PieceColor::White, PieceFigure::Pawn>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::White, PieceFigure::Bishop>())) {
            for (auto move :
                 MoveGenerator<PieceColor::White, PieceFigure::Bishop>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::White, PieceFigure::Rook>())) {
            for (auto move :
                 MoveGenerator<PieceColor::White, PieceFigure::Rook>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::White, PieceFigure::Knight>())) {
            for (auto move :
                 MoveGenerator<PieceColor::White, PieceFigure::Knight>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::White, PieceFigure::Queen>())) {
            for (auto move :
                 MoveGenerator<PieceColor::White, PieceFigure::Queen>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::White, PieceFigure::King>())) {
            for (auto move :
                 MoveGenerator<PieceColor::White, PieceFigure::King>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }
    }
    else {

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::Black, PieceFigure::Pawn>())) {
            for (auto move :
                 MoveGenerator<PieceColor::Black, PieceFigure::Pawn>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::Black, PieceFigure::Bishop>())) {
            for (auto move :
                 MoveGenerator<PieceColor::Black, PieceFigure::Bishop>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::Black, PieceFigure::Rook>())) {
            for (auto move :
                 MoveGenerator<PieceColor::Black, PieceFigure::Rook>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::Black, PieceFigure::Knight>())) {
            for (auto move :
                 MoveGenerator<PieceColor::Black, PieceFigure::Knight>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::Black, PieceFigure::Queen>())) {
            for (auto move :
                 MoveGenerator<PieceColor::Black, PieceFigure::Queen>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : PieceBitBoards::getSetBitPositions(
                 bitBoards.getPieceBitBoard<PieceColor::Black, PieceFigure::King>())) {
            for (auto move :
                 MoveGenerator<PieceColor::Black, PieceFigure::King>::generateLegalMoves(
                     bitBoards, static_cast<uint16_t>(origin))) {
                moves.push_back(std::move(move));
            }
        }
    }

    return moves;
}

} // namespace chessAi