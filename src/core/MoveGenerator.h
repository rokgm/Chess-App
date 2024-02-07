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

enum class MoveType
{
    Normal,
    Capture
};

class MoveGeneratorWrapper
{
public:
    template <MoveType TMoveType>
    static std::vector<Move> generateLegalMoves(const PieceBitBoards& bitBoards);
};

template <PieceColor TColor>
class MoveGenerator
{
public:
    template <MoveType TMoveType>
    inline static std::vector<Move> generateLegalMoves(const PieceBitBoards& bitBoards,
                                                       PieceFigure figure, uint16_t origin);

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
    static std::pair<bool, Move> isLegalMove(const PieceBitBoards& bitBoards, Move move,
                                             PieceFigure figure);

    inline static bool isKingInCheck(const PieceBitBoards& bitBoards);

    friend class MoveGeneratorWrapper;

private:
    /**
     * Handles attacks, pushes, en passant and promotion(WIP).
     */
    template <MoveType TMoveType>
    inline static std::vector<Move> generatePawnMoves(const PieceBitBoards& bitBoards,
                                                      uint16_t origin);
    template <MoveType TMoveType>
    inline static std::vector<Move> generateKnightMoves(const PieceBitBoards& bitBoards,
                                                        uint16_t origin);

    template <MoveType TMoveType>
    inline static std::vector<Move> generateKingMoves(const PieceBitBoards& bitBoards,
                                                      uint16_t origin, bool kingIsInCheck);

    template <PieceFigure TFigure, MoveType TMoveType>
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

template <PieceColor TColor>
const std::unique_ptr<magic_bits::Attacks>& MoveGenerator<TColor>::getMagicAttacks()
{
    if (magicAttacks != nullptr)
        return magicAttacks;
    magicAttacks = std::make_unique<magic_bits::Attacks>();
    return magicAttacks;
}

template <PieceColor TColor>
template <MoveType TMoveType>
inline std::vector<Move> MoveGenerator<TColor>::generateLegalMoves(const PieceBitBoards& bitBoards,
                                                                   PieceFigure figure,
                                                                   uint16_t origin)
{
    if (bitBoards.currentMoveColor != TColor) {
        return {};
    }

    if (figure == PieceFigure::Pawn)
        return generatePawnMoves<TMoveType>(bitBoards, origin);
    else if (figure == PieceFigure::Knight)
        return generateKnightMoves<TMoveType>(bitBoards, origin);
    else if (figure == PieceFigure::Bishop)
        return generateSlidingPieceMoves<PieceFigure::Bishop, TMoveType>(bitBoards, origin);
    else if (figure == PieceFigure::Rook)
        return generateSlidingPieceMoves<PieceFigure::Rook, TMoveType>(bitBoards, origin);
    else if (figure == PieceFigure::Queen)
        return generateSlidingPieceMoves<PieceFigure::Queen, TMoveType>(bitBoards, origin);
    else if (figure == PieceFigure::King)
        return generateKingMoves<TMoveType>(bitBoards, origin, isKingInCheck(bitBoards));
    else {
        CHESS_LOG_ERROR("Unhandled piece type.");
        return {};
    }
}

template <PieceColor TColor>
std::pair<bool, Move> MoveGenerator<TColor>::isLegalMove(const PieceBitBoards& bitBoards, Move move,
                                                         PieceFigure figure)
{
    auto moves = generateLegalMoves<MoveType::Normal>(bitBoards, figure, move.origin);
    // Use SpecialMoveCompare.
    for (const auto& generatedMove : moves) {
        SpecialMoveCompare compare(move);
        if (compare(generatedMove))
            return {true, generatedMove};
    }
    return {false, move};
}

template <PieceColor TColor>
template <MoveType TMoveType>
inline std::vector<Move> MoveGenerator<TColor>::generatePawnMoves(const PieceBitBoards& bitBoards,
                                                                  uint16_t origin)
{
    std::vector<Move> moves;

    // Generate basic attacks and pushes.
    auto allPieces = bitBoards.getAllPiecesBoard();
    auto legalAttacks =
        (Pawn<TColor>::originToAttacks[origin] & bitBoards.getAllOppositeColorPieces<TColor>());

    uint64_t legalOneSquarePushes;
    uint64_t legalTwoSquarePushes;
    if constexpr (TMoveType == MoveType::Normal) {
        legalOneSquarePushes = Pawn<TColor>::originToPushes[origin] & (~allPieces);
        legalTwoSquarePushes = 0;
        if ((allPieces & Pawn<TColor>::getFieldJumpedOverWithTwoPush(origin)) == 0) {
            legalTwoSquarePushes = Pawn<TColor>::originToTwoPushes[origin] & (~allPieces);
        }
    }
    else if constexpr (TMoveType == MoveType::Capture) {
        legalOneSquarePushes = 0;
        legalTwoSquarePushes = 0;
    }
    else
        static_assert(true, "Move type generation is not implemented.");

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
                appendMoveIfNoCheckHappens(moves, Move(origin, position, type, 1), bitBoards);
            }
        }
        else
            appendMoveIfNoCheckHappens(moves, Move(origin, position, 0, 0), bitBoards);
    }

    // En passant
    if (bitBoards.enPassantTargetSquare != 0) {
        uint64_t mask = 0;
        PieceBitBoards::setBit(mask, bitBoards.enPassantTargetSquare);
        for (auto destination :
             PieceBitBoards::getSetBitPositions(mask & Pawn<TColor>::originToAttacks[origin])) {
            appendMoveIfNoCheckHappens(moves, Move(origin, destination, 0, 2), bitBoards);
        }
    }
    return moves;
}

template <PieceColor TColor>
template <MoveType TMoveType>
inline std::vector<Move> MoveGenerator<TColor>::generateKnightMoves(const PieceBitBoards& bitBoards,
                                                                    uint16_t origin)
{
    std::vector<Move> moves;

    uint64_t maskOfAvailableSquares = 0;
    if constexpr (TMoveType == MoveType::Capture) {
        maskOfAvailableSquares = bitBoards.getAllOppositeColorPieces<TColor>();
    }
    else if constexpr (TMoveType == MoveType::Normal) {
        maskOfAvailableSquares = ~bitBoards.getAllPiecesBoard<TColor>();
    }
    else
        static_assert(true, "Move type generation is not implemented.");

    for (const auto& position : PieceBitBoards::getSetBitPositions(
             maskOfAvailableSquares & Knight::originToAttacks[origin])) {
        appendMoveIfNoCheckHappens(moves, Move(origin, position, 0, 0), bitBoards);
    }
    return moves;
}

template <PieceColor TColor>
template <MoveType TMoveType>
inline std::vector<Move> MoveGenerator<TColor>::generateKingMoves(const PieceBitBoards& bitBoards,
                                                                  uint16_t origin,
                                                                  bool kingIsInCheck)
{
    std::vector<Move> moves;
    auto tempBoards = bitBoards;
    // King can block attack of sliding piece, must be removed to get attacks through king.
    PieceBitBoards::clearBit(tempBoards.getModifiablePieceBitBoard<TColor, PieceFigure::King>(),
                             origin);
    auto attackOfAllOppositePieces = generateAttacksOfAllOppositePieces(tempBoards);

    uint64_t maskOfAvailableSquares = 0;
    if constexpr (TMoveType == MoveType::Capture) {
        maskOfAvailableSquares = bitBoards.getAllOppositeColorPieces<TColor>();
    }
    else if constexpr (TMoveType == MoveType::Normal) {
        maskOfAvailableSquares = ~bitBoards.getAllPiecesBoard<TColor>();
    }
    else
        static_assert(true, "Move type generation is not implemented.");

    for (const auto& position : PieceBitBoards::getSetBitPositions(
             maskOfAvailableSquares & King::originToAttacks[origin] & ~attackOfAllOppositePieces)) {
        moves.emplace_back(origin, position, static_cast<uint16_t>(0), static_cast<uint16_t>(0));
    }

    // Castling is illegal when in check and captures are not possible.
    if constexpr (TMoveType == MoveType::Capture)
        return moves;
    else {
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
}

template <PieceColor TColor>
template <PieceFigure TFigure, MoveType TMoveType>
inline std::vector<Move> MoveGenerator<TColor>::generateSlidingPieceMoves(
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

    uint64_t maskOfAvailableSquares = 0;
    if constexpr (TMoveType == MoveType::Capture) {
        maskOfAvailableSquares = bitBoards.getAllOppositeColorPieces<TColor>();
    }
    else if constexpr (TMoveType == MoveType::Normal) {
        maskOfAvailableSquares = ~bitBoards.getAllPiecesBoard<TColor>();
    }
    else
        static_assert(true, "Move type generation is not implemented.");

    std::vector<Move> moves;
    for (const auto& position :
         PieceBitBoards::getSetBitPositions(maskOfAvailableSquares & attacks)) {
        appendMoveIfNoCheckHappens(moves, Move(origin, position, 0, 0), bitBoards);
    }
    return moves;
}

template <PieceColor TColor>
uint64_t MoveGenerator<TColor>::generateAttacksOfAllOppositePieces(const PieceBitBoards& bitBoards)
{
    uint64_t attacks = 0;

    if constexpr (TColor == PieceColor::White) {
        for (auto position : bitBoards.blackPawnPositions) {
            attacks |= Pawn<PieceType::getOppositeColor<TColor>()>::originToAttacks[position];
        }
        for (auto position : bitBoards.blackKnightPositions) {
            attacks |= Knight::originToAttacks[position];
        }
        for (auto position : bitBoards.blackBishopPositions) {
            attacks |= getMagicAttacks()->Bishop(bitBoards.getAllPiecesBoard(),
                                                 static_cast<int>(position));
        }
        for (auto position : bitBoards.blackRookPositions) {
            attacks |=
                getMagicAttacks()->Rook(bitBoards.getAllPiecesBoard(), static_cast<int>(position));
        }
        for (auto position : bitBoards.blackQueenPositions) {
            attacks |=
                getMagicAttacks()->Queen(bitBoards.getAllPiecesBoard(), static_cast<int>(position));
        }
        if (!bitBoards.blackKingPositions.empty())
            attacks |= King::originToAttacks[bitBoards.blackKingPositions[0]];
    }
    else {
        for (auto position : bitBoards.whitePawnPositions) {
            attacks |= Pawn<PieceType::getOppositeColor<TColor>()>::originToAttacks[position];
        }
        for (auto position : bitBoards.whiteKnightPositions) {
            attacks |= Knight::originToAttacks[position];
        }
        for (auto position : bitBoards.whiteBishopPositions) {
            attacks |= getMagicAttacks()->Bishop(bitBoards.getAllPiecesBoard(),
                                                 static_cast<int>(position));
        }
        for (auto position : bitBoards.whiteRookPositions) {
            attacks |=
                getMagicAttacks()->Rook(bitBoards.getAllPiecesBoard(), static_cast<int>(position));
        }
        for (auto position : bitBoards.whiteQueenPositions) {
            attacks |=
                getMagicAttacks()->Queen(bitBoards.getAllPiecesBoard(), static_cast<int>(position));
        }
        if (!bitBoards.whiteKingPositions.empty())
            attacks |= King::originToAttacks[bitBoards.whiteKingPositions[0]];
    }

    return attacks;
}

template <PieceColor TColor>
bool MoveGenerator<TColor>::isKingInCheck(const PieceBitBoards& bitBoards)
{
    if constexpr (TColor == PieceColor::White) {
        return generateAttacksOfAllOppositePieces(bitBoards) & bitBoards.whiteKing;
    }
    else {
        return generateAttacksOfAllOppositePieces(bitBoards) & bitBoards.blackKing;
    }
}

template <PieceColor TColor>
void MoveGenerator<TColor>::appendMoveIfNoCheckHappens(std::vector<Move>& moves, Move move,
                                                       const PieceBitBoards& bitBoards)
{
    PieceBitBoards temporaryBitBoards = bitBoards;
    temporaryBitBoards.applyMove(move);
    if (!isKingInCheck(temporaryBitBoards))
        moves.push_back(std::move(move));
}

template <MoveType TMoveType>
std::vector<Move> MoveGeneratorWrapper::generateLegalMoves(const PieceBitBoards& bitBoards)
{
    std::vector<Move> moves;

    if (bitBoards.whiteKingPositions.empty() || bitBoards.blackKingPositions.empty()) {
        CHESS_LOG_ERROR("Empty king position.");
        return moves;
    }

    if (bitBoards.currentMoveColor == PieceColor::White) {
        for (auto origin : bitBoards.whitePawnPositions) {
            for (auto move : MoveGenerator<PieceColor::White>::generatePawnMoves<TMoveType>(
                     bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.whiteBishopPositions) {
            for (auto move : MoveGenerator<PieceColor::White>::generateSlidingPieceMoves<
                     PieceFigure::Bishop, TMoveType>(bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.whiteRookPositions) {
            for (auto move : MoveGenerator<PieceColor::White>::generateSlidingPieceMoves<
                     PieceFigure::Rook, TMoveType>(bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.whiteKnightPositions) {
            for (auto move : MoveGenerator<PieceColor::White>::generateKnightMoves<TMoveType>(
                     bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.whiteQueenPositions) {
            for (auto move : MoveGenerator<PieceColor::White>::generateSlidingPieceMoves<
                     PieceFigure::Queen, TMoveType>(bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto move : MoveGenerator<PieceColor::White>::generateKingMoves<TMoveType>(
                 bitBoards, bitBoards.whiteKingPositions[0],
                 MoveGenerator<PieceColor::White>::isKingInCheck(bitBoards))) {
            moves.push_back(std::move(move));
        }
    }
    else {
        for (auto origin : bitBoards.blackPawnPositions) {
            for (auto move : MoveGenerator<PieceColor::Black>::generatePawnMoves<TMoveType>(
                     bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.blackBishopPositions) {
            for (auto move : MoveGenerator<PieceColor::Black>::generateSlidingPieceMoves<
                     PieceFigure::Bishop, TMoveType>(bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.blackRookPositions) {
            for (auto move : MoveGenerator<PieceColor::Black>::generateSlidingPieceMoves<
                     PieceFigure::Rook, TMoveType>(bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.blackKnightPositions) {
            for (auto move : MoveGenerator<PieceColor::Black>::generateKnightMoves<TMoveType>(
                     bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto origin : bitBoards.blackQueenPositions) {
            for (auto move : MoveGenerator<PieceColor::Black>::generateSlidingPieceMoves<
                     PieceFigure::Queen, TMoveType>(bitBoards, origin)) {
                moves.push_back(std::move(move));
            }
        }

        for (auto move : MoveGenerator<PieceColor::Black>::generateKingMoves<TMoveType>(
                 bitBoards, bitBoards.blackKingPositions[0],
                 MoveGenerator<PieceColor::Black>::isKingInCheck(bitBoards))) {
            moves.push_back(std::move(move));
        }
    }

    return moves;
}

} // namespace chessAi