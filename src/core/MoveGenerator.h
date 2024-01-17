#pragma once

#include "King.h"
#include "Knight.h"
#include "Move.h"
#include "Pawn.h"
#include "PieceBitBoards.h"
#include "PieceType.h"
#include "magic-bits-master/include/magic_bits.hpp"

#include <algorithm>
#include <unordered_map>

namespace chessAi
{

class MoveGeneratorWrapper
{
public:
    static std::vector<Move> generateLegalMoves(const PieceBitBoards& bitBoards);
};

template <PieceColor TColor, PieceFigure TFigure = PieceFigure::Empty>
class MoveGenerator
{
public:
    inline static std::vector<Move> generateLegalMoves(const PieceBitBoards& bitBoards,
                                                       uint16_t origin);

    /**
     * Searches all generated legal moves int given position and returns if move matches one.
     * When comparing two moves only origin and destination of moves are checked for match.
     * (If king moved 2 squares to the king side, that must be a king side castle. Special flag
     * doesn't need to match. Same for en Passant, if a move matches a generated move (which has
     * an en passant flag), then move was also an en passant.)
     *
     * If any of two moves has promotion flag set, then PROMOTION TYPE and SPECIAL FLAGS are also
     * checked when comparing. We have to now which promotion happened, so we return the correct
     * generated move.
     *
     * @return true if move is legal and the generated move that matched the move which was checked.
     * Returned generated move has special flags set, even if original move did not have  (castling,
     * en passant).
     */
    static std::pair<bool, Move> isLegalMove(const PieceBitBoards& bitBoards, Move move);

    inline static bool isKingInCheck(const PieceBitBoards& bitBoards);

private:
    /**
     * Handles attacks, pushes, en passant and promotion(WIP).
     */
    inline static std::vector<Move> generatePawnMoves(const PieceBitBoards& bitBoards,
                                                      uint16_t origin);
    inline static std::vector<Move> generateKnightMoves(const PieceBitBoards& bitBoards,
                                                        uint16_t origin);
    inline static std::vector<Move> generateKingMoves(const PieceBitBoards& bitBoards,
                                                      uint16_t origin, bool kingIsInCheck);

    inline static std::vector<Move> generateSlidingPieceMoves(const PieceBitBoards& bitBoards,
                                                              uint16_t origin);

    inline static const std::unique_ptr<magic_bits::Attacks>& getMagicAttacks();

    inline static uint64_t generateAttacksOfAllOppositePieces(const PieceBitBoards& bitBoards);

    /**
     * Append move if move doesn't result in king check (king was already in check or piece is
     * pinned).
     */
    inline static void appendMoveIfNoCheckHappens(std::vector<Move>& moves, Move move,
                                                  const PieceBitBoards& bitBoards);

private:
    inline static std::unique_ptr<magic_bits::Attacks> magicAttacks = nullptr;
};

template <PieceColor TColor, PieceFigure TFigure>
const std::unique_ptr<magic_bits::Attacks>& MoveGenerator<TColor, TFigure>::getMagicAttacks()
{
    if (magicAttacks != nullptr)
        return magicAttacks;
    magicAttacks = std::make_unique<magic_bits::Attacks>();
    return magicAttacks;
}

template <PieceColor TColor, PieceFigure TFigure>
inline std::vector<Move> MoveGenerator<TColor, TFigure>::generateLegalMoves(
    const PieceBitBoards& bitBoards, uint16_t origin)
{
    if (bitBoards.currentMoveColor != TColor) {
        return {};
    }

    if constexpr (TFigure == PieceFigure::Pawn)
        return generatePawnMoves(bitBoards, origin);
    else if constexpr (TFigure == PieceFigure::Knight)
        return generateKnightMoves(bitBoards, origin);
    else if constexpr (TFigure == PieceFigure::Bishop || TFigure == PieceFigure::Rook ||
                       TFigure == PieceFigure::Queen)
        return generateSlidingPieceMoves(bitBoards, origin);
    else if constexpr (TFigure == PieceFigure::King)
        return generateKingMoves(bitBoards, origin, isKingInCheck(bitBoards));
    else {
        CHESS_LOG_ERROR("Unhandled piece type.");
        return {};
    }
}

template <PieceColor TColor, PieceFigure TFigure>
std::pair<bool, Move> MoveGenerator<TColor, TFigure>::isLegalMove(const PieceBitBoards& bitBoards,
                                                                  Move move)
{
    auto moves = generateLegalMoves(bitBoards, move.origin);
    // Use SpecialMoveCompare.
    for (const auto& generatedMove : moves) {
        SpecialMoveCompare compare(move);
        if (compare(generatedMove))
            return {true, generatedMove};
    }
    return {false, move};
}

template <PieceColor TColor, PieceFigure TFigure>
inline std::vector<Move> MoveGenerator<TColor, TFigure>::generatePawnMoves(
    const PieceBitBoards& bitBoards, uint16_t origin)
{
    std::vector<Move> moves;

    // Generate basic attacks and pushes.
    auto allPieces = bitBoards.getAllPiecesBoard();
    auto legalAttacks =
        (Pawn<TColor>::originToAttacks[origin] & bitBoards.getAllOppositeColorPieces<TColor>());
    auto legalOneSquarePushes = Pawn<TColor>::originToPushes[origin] & (~allPieces);
    uint64_t legalTwoSquarePushes = 0;
    if ((allPieces & Pawn<TColor>::getFieldJumpedOverWithTwoPush(origin)) == 0) {
        legalTwoSquarePushes = Pawn<TColor>::originToTwoPushes[origin] & (~allPieces);
    }
    // Check if on promotion rank.
    bool promotion = false;
    if constexpr (TColor == PieceColor::White) {
        if (origin / 8 == 1)
            promotion = true;
    }
    if constexpr (TColor == PieceColor::Black) {
        if (origin / 8 == 6)
            promotion = true;
    }
    for (const auto& position : PieceBitBoards::getSetBitPositions(
             ~bitBoards.getAllPiecesBoard<TColor>() &
             (legalAttacks | legalOneSquarePushes | legalTwoSquarePushes | legalTwoSquarePushes))) {
        if (promotion) {
            for (uint16_t type = 0; type < 4; type++) {
                appendMoveIfNoCheckHappens(
                    moves, Move(origin, static_cast<uint16_t>(position), type, 1), bitBoards);
            }
        }
        else
            appendMoveIfNoCheckHappens(moves, Move(origin, static_cast<uint16_t>(position), 0, 0),
                                       bitBoards);
    }

    // En passant
    if (bitBoards.enPassantTargetSquare != 0) {
        uint64_t mask = 0;
        PieceBitBoards::setBit(mask, bitBoards.enPassantTargetSquare);
        for (auto destination :
             PieceBitBoards::getSetBitPositions(mask & Pawn<TColor>::originToAttacks[origin])) {
            appendMoveIfNoCheckHappens(
                moves, Move(origin, static_cast<uint16_t>(destination), 0, 2), bitBoards);
        }
    }
    return moves;
}

template <PieceColor TColor, PieceFigure TFigure>
inline std::vector<Move> MoveGenerator<TColor, TFigure>::generateKnightMoves(
    const PieceBitBoards& bitBoards, uint16_t origin)
{
    std::vector<Move> moves;
    for (const auto& position : PieceBitBoards::getSetBitPositions(
             ~bitBoards.getAllPiecesBoard<TColor>() & Knight::originToAttacks[origin])) {
        appendMoveIfNoCheckHappens(moves, Move(origin, static_cast<uint16_t>(position), 0, 0),
                                   bitBoards);
    }
    return moves;
}

template <PieceColor TColor, PieceFigure TFigure>
inline std::vector<Move> MoveGenerator<TColor, TFigure>::generateKingMoves(
    const PieceBitBoards& bitBoards, uint16_t origin, bool kingIsInCheck)
{
    std::vector<Move> moves;
    auto tempBoards = bitBoards;
    // King can block attack of sliding piece, must be removed to get attacks through king.
    PieceBitBoards::clearBit(tempBoards.getModifiablePieceBitBoard<TColor, PieceFigure::King>(),
                             origin);
    auto attackOfAllOppositePieces = generateAttacksOfAllOppositePieces(tempBoards);

    for (const auto& position : PieceBitBoards::getSetBitPositions(
             ~bitBoards.getAllPiecesBoard<TColor>() & King::originToAttacks[origin] &
             ~attackOfAllOppositePieces)) {
        moves.emplace_back(origin, static_cast<uint16_t>(position), static_cast<uint16_t>(0),
                           static_cast<uint16_t>(0));
    }

    // Castling is illegal when in check.
    if (kingIsInCheck)
        return moves;

    // Castling
    bool canKingSideCastle = false;
    bool canQueenSideCastle = false;
    uint64_t kingSideMask = 0;
    uint64_t queenSideAttackedMask = 0;
    uint64_t queenSidePiecesMask = 0;
    uint16_t destinationKingSide = 0;
    uint16_t destinationQueenSide = 0;
    if constexpr (TColor == PieceColor::White) {
        canKingSideCastle = bitBoards.whiteKingSideCastle;
        canQueenSideCastle = bitBoards.whiteQueenSideCastle;
        kingSideMask = King::whiteKingSideCastleMask;
        queenSideAttackedMask = King::whiteQueenSideCastleAttackedMask;
        queenSidePiecesMask = King::whiteQueenSideCastlePiecesMask;
        destinationKingSide = 62;
        destinationQueenSide = 58;
    }
    else {
        canKingSideCastle = bitBoards.blackKingSideCastle;
        canQueenSideCastle = bitBoards.blackQueenSideCastle;
        kingSideMask = King::blackKingSideCastleMask;
        queenSideAttackedMask = King::blackQueenSideCastleAttackedMask;
        queenSidePiecesMask = King::blackQueenSideCastlePiecesMask;
        destinationKingSide = 6;
        destinationQueenSide = 2;
    }
    auto allPieces = bitBoards.getAllPiecesBoard();
    if (canKingSideCastle) {
        if (((kingSideMask & attackOfAllOppositePieces) | (kingSideMask & allPieces)) == 0)
            moves.emplace_back(origin, destinationKingSide, static_cast<uint16_t>(0),
                               static_cast<uint16_t>(3));
    }
    if (canQueenSideCastle) {
        if (((queenSideAttackedMask & attackOfAllOppositePieces) |
             (queenSidePiecesMask & allPieces)) == 0)
            moves.emplace_back(origin, destinationQueenSide, static_cast<uint16_t>(0),
                               static_cast<uint16_t>(3));
    }

    return moves;
}

template <PieceColor TColor, PieceFigure TFigure>
inline std::vector<Move> MoveGenerator<TColor, TFigure>::generateSlidingPieceMoves(
    const PieceBitBoards& bitBoards, uint16_t origin)
{
    uint64_t attacks = 0;
    if constexpr (TFigure == PieceFigure::Bishop)
        attacks =
            getMagicAttacks()->Bishop(bitBoards.getAllPiecesBoard(), static_cast<int>(origin));
    else if constexpr (TFigure == PieceFigure::Rook)
        attacks = getMagicAttacks()->Rook(bitBoards.getAllPiecesBoard(), static_cast<int>(origin));
    else if constexpr (TFigure == PieceFigure::Queen)
        attacks = getMagicAttacks()->Queen(bitBoards.getAllPiecesBoard(), static_cast<int>(origin));

    std::vector<Move> moves;
    for (const auto& position :
         PieceBitBoards::getSetBitPositions(~bitBoards.getAllPiecesBoard<TColor>() & attacks)) {
        appendMoveIfNoCheckHappens(moves, Move(origin, static_cast<uint16_t>(position), 0, 0),
                                   bitBoards);
    }
    return moves;
}

template <PieceColor TColor, PieceFigure TFigure>
uint64_t MoveGenerator<TColor, TFigure>::generateAttacksOfAllOppositePieces(
    const PieceBitBoards& bitBoards)
{
    uint64_t attacks = 0;
    uint64_t pawns = 0;
    uint64_t knights = 0;
    uint64_t bishops = 0;
    uint64_t rooks = 0;
    uint64_t queens = 0;
    uint64_t king = 0;

    if constexpr (TColor == PieceColor::White) {
        pawns = bitBoards.blackPawns;
        knights = bitBoards.blackKnights;
        bishops = bitBoards.blackBishops;
        rooks = bitBoards.blackRooks;
        queens = bitBoards.blackQueens;
        king = bitBoards.blackKing;
    }
    else {
        pawns = bitBoards.whitePawns;
        knights = bitBoards.whiteKnights;
        bishops = bitBoards.whiteBishops;
        rooks = bitBoards.whiteRooks;
        queens = bitBoards.whiteQueens;
        king = bitBoards.whiteKing;
    }

    for (const auto& position : PieceBitBoards::getSetBitPositions(pawns)) {
        attacks |= Pawn<PieceType::getOppositeColor<TColor>()>::originToAttacks[position];
    }
    for (const auto& position : PieceBitBoards::getSetBitPositions(knights)) {
        attacks |= Knight::originToAttacks[position];
    }
    for (const auto& position : PieceBitBoards::getSetBitPositions(bishops)) {
        attacks |=
            getMagicAttacks()->Bishop(bitBoards.getAllPiecesBoard(), static_cast<int>(position));
    }
    for (const auto& position : PieceBitBoards::getSetBitPositions(rooks)) {
        attacks |=
            getMagicAttacks()->Rook(bitBoards.getAllPiecesBoard(), static_cast<int>(position));
    }
    for (const auto& position : PieceBitBoards::getSetBitPositions(queens)) {
        attacks |=
            getMagicAttacks()->Queen(bitBoards.getAllPiecesBoard(), static_cast<int>(position));
    }
    for (const auto& position : PieceBitBoards::getSetBitPositions(king)) {
        attacks |= King::originToAttacks[position];
    }
    return attacks;
}

template <PieceColor TColor, PieceFigure TFigure>
bool MoveGenerator<TColor, TFigure>::isKingInCheck(const PieceBitBoards& bitBoards)
{
    if constexpr (TColor == PieceColor::White) {
        return generateAttacksOfAllOppositePieces(bitBoards) & bitBoards.whiteKing;
    }
    else {
        return generateAttacksOfAllOppositePieces(bitBoards) & bitBoards.blackKing;
    }
}

template <PieceColor TColor, PieceFigure TFigure>
void MoveGenerator<TColor, TFigure>::appendMoveIfNoCheckHappens(std::vector<Move>& moves, Move move,
                                                                const PieceBitBoards& bitBoards)
{
    PieceBitBoards temporaryBitBoards = bitBoards;
    temporaryBitBoards.applyMove(move);
    if (!isKingInCheck(temporaryBitBoards))
        moves.push_back(std::move(move));
}

} // namespace chessAi