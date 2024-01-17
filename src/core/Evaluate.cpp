#include "Evaluate.h"

#include <algorithm>

namespace chessAi
{

int Evaluate::mopUpEvaluation(const PieceBitBoards& boards)
{
    int evaluation = 0;

    unsigned int kingPosition = 0;
    unsigned int opponentsKingPosition = 0;
    if (boards.currentMoveColor == PieceColor::White) {
        for (auto position : PieceBitBoards::getSetBitPositions(boards.whiteKing)) {
            kingPosition = position;
        }
        for (auto position : PieceBitBoards::getSetBitPositions(boards.blackKing)) {
            opponentsKingPosition = position;
        }
    }
    else {
        for (auto position : PieceBitBoards::getSetBitPositions(boards.blackKing)) {
            kingPosition = position;
        }
        for (auto position : PieceBitBoards::getSetBitPositions(boards.whiteKing)) {
            opponentsKingPosition = position;
        }
    }

    // TODO: Make this precomputed.
    unsigned int manhattanDistance =
        abs(static_cast<int>(kingPosition % 8) - static_cast<int>(opponentsKingPosition % 8)) +
        abs(static_cast<int>(kingPosition / 8) - static_cast<int>(opponentsKingPosition / 8));
    evaluation += static_cast<int>(1.6f * static_cast<float>(14 - manhattanDistance));

    evaluation +=
        static_cast<int>(4.7f * static_cast<float>(centerManhattanDistance[opponentsKingPosition]));
    return static_cast<int>(s_endgameWeight * static_cast<float>(evaluation));
}

int Evaluate::pieceSquareTableEvaluation(const PieceBitBoards& boards)
{
    int evaluation = 0;

    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.whitePawns)) {
        evaluation += pawnSquareValues[position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.whiteKnights)) {
        evaluation += knightSquareValues[position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.whiteBishops)) {
        evaluation += bishopSquareValues[position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.whiteRooks)) {
        evaluation += rookSquareValues[position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.whiteQueens)) {
        evaluation += queenSquareValues[position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.whiteKing)) {
        evaluation += static_cast<int>((1 - s_endgameWeight) *
                                       static_cast<float>(kingMiddleGameSquareValues[position])) +
                      static_cast<int>(s_endgameWeight *
                                       static_cast<float>(kingEndGameSquareValues[position]));
    }

    // Black
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.blackPawns)) {
        evaluation -= pawnSquareValues[63 - position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.blackKnights)) {
        evaluation -= knightSquareValues[63 - position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.blackBishops)) {
        evaluation -= bishopSquareValues[63 - position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.blackRooks)) {
        evaluation -= rookSquareValues[63 - position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.blackQueens)) {
        evaluation -= queenSquareValues[63 - position];
    }
    for (unsigned int position : PieceBitBoards::getSetBitPositions(boards.blackKing)) {
        evaluation -=
            static_cast<int>((1 - s_endgameWeight) *
                             static_cast<float>(kingMiddleGameSquareValues[63 - position])) +
            static_cast<int>(s_endgameWeight *
                             static_cast<float>(kingEndGameSquareValues[63 - position]));
    }
    return evaluation;
}

int Evaluate::getEvaluation(const PieceBitBoards& boards)
{
    s_endgameWeight = endgameWeight(boards);

    int whiteEvaluation = 0;
    int blackEvaluation = 0;

    whiteEvaluation += PieceBitBoards::countSetBits(boards.whitePawns) * s_pawnValue;
    whiteEvaluation += PieceBitBoards::countSetBits(boards.whiteBishops) * s_bishopValue;
    whiteEvaluation += PieceBitBoards::countSetBits(boards.whiteKnights) * s_knightValue;
    whiteEvaluation += PieceBitBoards::countSetBits(boards.whiteRooks) * s_rookValue;
    whiteEvaluation += PieceBitBoards::countSetBits(boards.whiteQueens) * s_queenValue;

    blackEvaluation += PieceBitBoards::countSetBits(boards.blackPawns) * s_pawnValue;
    blackEvaluation += PieceBitBoards::countSetBits(boards.blackBishops) * s_bishopValue;
    blackEvaluation += PieceBitBoards::countSetBits(boards.blackKnights) * s_knightValue;
    blackEvaluation += PieceBitBoards::countSetBits(boards.blackRooks) * s_rookValue;
    blackEvaluation += PieceBitBoards::countSetBits(boards.blackQueens) * s_queenValue;

    int evaluation = whiteEvaluation - blackEvaluation;

    if (whiteEvaluation > blackEvaluation + 2 * s_pawnValue)
        evaluation += mopUpEvaluation(boards);
    else if (blackEvaluation > whiteEvaluation + 2 * s_pawnValue)
        evaluation -= mopUpEvaluation(boards);

    evaluation += pieceSquareTableEvaluation(boards);

    return (boards.currentMoveColor == PieceColor::White) ? evaluation : -evaluation;
}

int Evaluate::getFigureValue(PieceFigure figure)
{
    switch (figure) {
    case PieceFigure::Pawn:
        return s_pawnValue;
    case PieceFigure::Bishop:
        return s_bishopValue;
    case PieceFigure::Knight:
        return s_knightValue;
    case PieceFigure::Rook:
        return s_rookValue;
    case PieceFigure::Queen:
        return s_queenValue;
    default:
        return 0;
    }
}

float Evaluate::endgameWeight(const PieceBitBoards& boards)
{
    float endGameStart =
        1 / static_cast<float>(s_rookValue + s_bishopValue + s_knightValue + s_knightValue);

    int materialCountNoPawns =
        std::min(PieceBitBoards::countSetBits(boards.whiteKnights) * s_knightValue +
                     PieceBitBoards::countSetBits(boards.whiteBishops) * s_bishopValue +
                     PieceBitBoards::countSetBits(boards.whiteRooks) * s_rookValue +
                     PieceBitBoards::countSetBits(boards.whiteQueens) * s_queenValue,
                 PieceBitBoards::countSetBits(boards.blackKnights) * s_knightValue +
                     PieceBitBoards::countSetBits(boards.blackBishops) * s_bishopValue +
                     PieceBitBoards::countSetBits(boards.blackRooks) * s_rookValue +
                     PieceBitBoards::countSetBits(boards.blackQueens) * s_queenValue);

    return 1.f - std::min(1.f, endGameStart * static_cast<float>(materialCountNoPawns));
}

} // namespace chessAi