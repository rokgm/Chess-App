#include "Evaluate.h"

#include <algorithm>

namespace chessAi
{

int Evaluate::mopUpEvaluation(const PieceBitBoards& boards)
{
    if (std::abs(s_endgameWeight) < 0.01f)
        return 0;

    int evaluation = 0;

    unsigned int kingPosition = 0;
    unsigned int opponentsKingPosition = 0;

    if (boards.whiteKingPositions.empty() || boards.blackKingPositions.empty()) {
        CHESS_LOG_ERROR("Empty king position.");
        return 0;
    }

    if (boards.currentMoveColor == PieceColor::White) {
        kingPosition = boards.whiteKingPositions[0];
        opponentsKingPosition = boards.blackKingPositions[0];
    }
    else {
        kingPosition = boards.blackKingPositions[0];
        opponentsKingPosition = boards.whiteKingPositions[0];
    }

    evaluation += static_cast<int>(
        1.6f * static_cast<float>(14 - s_manhattanDistance[kingPosition][opponentsKingPosition]));

    evaluation += static_cast<int>(
        4.7f * static_cast<float>(s_centerManhattanDistance[opponentsKingPosition]));
    return static_cast<int>(s_endgameWeight * static_cast<float>(evaluation));
}

int Evaluate::pieceSquareTableEvaluation(const PieceBitBoards& boards)
{
    int evaluation = 0;

    for (unsigned int position : boards.whitePawnPositions) {
        evaluation += s_pawnSquareValues[position];
    }
    for (unsigned int position : boards.whiteKnightPositions) {
        evaluation += s_knightSquareValues[position];
    }
    for (unsigned int position : boards.whiteBishopPositions) {
        evaluation += s_bishopSquareValues[position];
    }
    for (unsigned int position : boards.whiteRookPositions) {
        evaluation += s_rookSquareValues[position];
    }
    for (unsigned int position : boards.whiteQueenPositions) {
        evaluation += s_queenSquareValues[position];
    }

    if (boards.whiteKingPositions.empty() || boards.blackKingPositions.empty()) {
        CHESS_LOG_ERROR("Empty king position.");
        return 0;
    }

    evaluation +=
        static_cast<int>(
            (1 - s_endgameWeight) *
            static_cast<float>(s_kingMiddleGameSquareValues[boards.whiteKingPositions[0]])) +
        static_cast<int>(
            s_endgameWeight *
            static_cast<float>(s_kingEndGameSquareValues[boards.whiteKingPositions[0]]));

    // Black
    for (unsigned int position : boards.blackPawnPositions) {
        evaluation -= s_pawnSquareValues[63 - position];
    }
    for (unsigned int position : boards.blackKnightPositions) {
        evaluation -= s_knightSquareValues[63 - position];
    }
    for (unsigned int position : boards.blackBishopPositions) {
        evaluation -= s_bishopSquareValues[63 - position];
    }
    for (unsigned int position : boards.blackRookPositions) {
        evaluation -= s_rookSquareValues[63 - position];
    }
    for (unsigned int position : boards.blackQueenPositions) {
        evaluation -= s_queenSquareValues[63 - position];
    }

    evaluation -=
        static_cast<int>(
            (1 - s_endgameWeight) *
            static_cast<float>(s_kingMiddleGameSquareValues[63 - boards.blackKingPositions[0]])) +
        static_cast<int>(
            s_endgameWeight *
            static_cast<float>(s_kingEndGameSquareValues[63 - boards.blackKingPositions[0]]));

    return evaluation;
}

int Evaluate::getEvaluation(const PieceBitBoards& boards)
{
    s_endgameWeight = endgameWeight(boards);

    int whiteEvaluation = 0;
    int blackEvaluation = 0;

    whiteEvaluation += static_cast<int>(boards.whitePawnPositions.size()) * s_pawnValue;
    whiteEvaluation += static_cast<int>(boards.whiteBishopPositions.size()) * s_bishopValue;
    whiteEvaluation += static_cast<int>(boards.whiteKnightPositions.size()) * s_knightValue;
    whiteEvaluation += static_cast<int>(boards.whiteRookPositions.size()) * s_rookValue;
    whiteEvaluation += static_cast<int>(boards.whiteQueenPositions.size()) * s_queenValue;

    blackEvaluation += static_cast<int>(boards.blackPawnPositions.size()) * s_pawnValue;
    blackEvaluation += static_cast<int>(boards.blackBishopPositions.size()) * s_bishopValue;
    blackEvaluation += static_cast<int>(boards.blackKnightPositions.size()) * s_knightValue;
    blackEvaluation += static_cast<int>(boards.blackRookPositions.size()) * s_rookValue;
    blackEvaluation += static_cast<int>(boards.blackQueenPositions.size()) * s_queenValue;

    int evaluation = whiteEvaluation - blackEvaluation;

    if (whiteEvaluation > blackEvaluation + 2 * s_pawnValue)
        evaluation += mopUpEvaluation(boards);
    else if (blackEvaluation > whiteEvaluation + 2 * s_pawnValue)
        evaluation -= mopUpEvaluation(boards);

    evaluation += pieceSquareTableEvaluation(boards);
    evaluation += kingPawnShield(boards);

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

int Evaluate::kingPawnShield(const PieceBitBoards& boards)
{
    int evaluation = 0;

    // If in endgame, pawn shield is not evaluated.
    if (std::abs(s_endgameWeight) > 0.f)
        return evaluation;

    // If the king is castled, we add a penalty if no pawn shield.
    // King on g1: pawn must be on f2; pawn must be on either of g2 or g3; pawn must be on
    // either of h2 or h3.

    // White king side castle
    if (boards.whiteKing & 0b1100000000000000000000000000000000000000000000000000000000000000) {
        // f2
        if ((boards.whitePawns & 0b00100000000000000000000000000000000000000000000000000000) == 0)
            evaluation -= 40;
        // g2 or g3
        if ((boards.whitePawns & 0b01000000010000000000000000000000000000000000000000000000) == 0)
            evaluation -= 40;
        // h2 or h3
        if ((boards.whitePawns & 0b10000000100000000000000000000000000000000000000000000000) == 0)
            evaluation -= 40;
    }
    // White queen side castle
    else if (boards.whiteKing & 0b11100000000000000000000000000000000000000000000000000000000) {
        // a2 or a3
        if ((boards.whitePawns & 0b1000000010000000000000000000000000000000000000000) == 0)
            evaluation -= 40;
        // b2
        if ((boards.whitePawns & 0b10000000000000000000000000000000000000000000000000) == 0)
            evaluation -= 40;
        // c2
        if ((boards.whitePawns & 0b100000000000000000000000000000000000000000000000000) == 0)
            evaluation -= 40;
        // d2
        if ((boards.whitePawns & 0b1000000000000000000000000000000000000000000000000000) == 0)
            evaluation -= 40;
    }

    // Black king side castle
    if (boards.blackKing & 0b11000000) {
        // f7
        if ((boards.blackPawns & 0b10000000000000) == 0)
            evaluation += 40;
        // g2 or g3
        if ((boards.blackPawns & 0b010000000100000000000000) == 0)
            evaluation += 40;
        // h2 or h3
        if ((boards.blackPawns & 0b100000001000000000000000) == 0)
            evaluation += 40;
    }
    // Black queen side castle
    else if (boards.blackKing & 0b111) {
        // a7 or a6
        if ((boards.blackPawns & 0b10000000100000000) == 0)
            evaluation += 40;
        // b7
        if ((boards.blackPawns & 0b1000000000) == 0)
            evaluation += 40;
        // c7
        if ((boards.blackPawns & 0b10000000000) == 0)
            evaluation += 40;
        // d7
        if ((boards.blackPawns & 0b100000000000) == 0)
            evaluation += 40;
    }

    return evaluation;
}

float Evaluate::endgameWeight(const PieceBitBoards& boards)
{
    float endGameStart =
        1 / static_cast<float>(s_rookValue + s_bishopValue + s_knightValue + s_knightValue);

    int materialCountNoPawns =
        std::min(static_cast<int>(boards.whiteKnightPositions.size()) * s_knightValue +
                     static_cast<int>(boards.whiteBishopPositions.size()) * s_bishopValue +
                     static_cast<int>(boards.whiteRookPositions.size()) * s_rookValue +
                     static_cast<int>(boards.whiteQueenPositions.size()) * s_queenValue,
                 static_cast<int>(boards.blackKnightPositions.size()) * s_knightValue +
                     static_cast<int>(boards.blackBishopPositions.size()) * s_bishopValue +
                     static_cast<int>(boards.blackRookPositions.size()) * s_rookValue +
                     static_cast<int>(boards.blackQueenPositions.size()) * s_queenValue);

    return 1.f - std::min(1.f, endGameStart * static_cast<float>(materialCountNoPawns));
}

std::array<std::array<int, 64>, 64> Evaluate::precalculateManhattanDistance()
{
    std::array<std::array<int, 64>, 64> distance{};

    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 64; y++) {
            distance[x][y] = abs(x % 8 - y % 8) + abs(x / 8 - y / 8);
        }
    }

    return distance;
}

} // namespace chessAi