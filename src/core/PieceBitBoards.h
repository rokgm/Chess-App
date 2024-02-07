#pragma once

#include "Move.h"
#include "PieceType.h"
#include "logger/Logger.h"

#include <map>
#include <set>
#include <string>

namespace chessAi
{

struct PieceBitBoards
{
    /**
     * Position count starts at top left of the board (a8), then goes horizontally from left
     * to right on ranks.
     * Default position is the starting position in chess.
     */
    PieceBitBoards(
        const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    uint64_t whitePawns = 0;
    uint64_t whiteBishops = 0;
    uint64_t whiteKnights = 0;
    uint64_t whiteRooks = 0;
    uint64_t whiteQueens = 0;
    uint64_t whiteKing = 0;
    uint64_t blackPawns = 0;
    uint64_t blackBishops = 0;
    uint64_t blackKnights = 0;
    uint64_t blackRooks = 0;
    uint64_t blackQueens = 0;
    uint64_t blackKing = 0;

    // En passant target square stored the same way as in fen.
    uint16_t enPassantTargetSquare = 0;

    bool whiteKingSideCastle = true;
    bool whiteQueenSideCastle = true;
    bool blackKingSideCastle = true;
    bool blackQueenSideCastle = true;

    PieceColor currentMoveColor = PieceColor::White;

    unsigned int halfMoveCount = 0;

    uint64_t zobristKey = 0;

    // Contigious (iterating over this many times). Works faster than set or unordered set,
    // otherwise set would make more sense.
    std::vector<uint16_t> whitePawnPositions;
    std::vector<uint16_t> whiteBishopPositions;
    std::vector<uint16_t> whiteKnightPositions;
    std::vector<uint16_t> whiteRookPositions;
    std::vector<uint16_t> whiteQueenPositions;
    std::vector<uint16_t> whiteKingPositions;

    std::vector<uint16_t> blackPawnPositions;
    std::vector<uint16_t> blackBishopPositions;
    std::vector<uint16_t> blackKnightPositions;
    std::vector<uint16_t> blackRookPositions;
    std::vector<uint16_t> blackQueenPositions;
    std::vector<uint16_t> blackKingPositions;

public:
    /**
     * Apply move to bit boards, update castling rights and updates current move color.
     */
    void applyMove(Move move);

    inline std::map<PieceType, const uint64_t*> getTypeToPieceBitBoards() const;

    inline static void setBit(uint64_t& number, uint16_t index);

    inline static bool getBit(uint64_t number, uint16_t index);

    inline static void clearBit(uint64_t& number, uint16_t index);

    inline static std::vector<uint16_t> getSetBitPositions(uint64_t number);

    inline static uint16_t countSetBits(uint64_t number);

    inline uint64_t getAllPiecesBoard() const;

    template <PieceColor TColor>
    inline uint64_t getAllPiecesBoard() const;

    template <PieceColor TColor, PieceFigure TFigure>
    inline constexpr uint64_t getPieceBitBoard() const;

    uint64_t getPieceBitBoard(const PieceType& type) const;

    template <PieceColor TColor>
    inline uint64_t getAllOppositeColorPieces() const;

    template <typename TBitWiseOperator>
    inline PieceType modifyAllBitBoards(uint64_t mask, TBitWiseOperator bitWiseOperator);

    template <PieceColor TColor, PieceFigure TFigure>
    inline uint64_t& getModifiablePieceBitBoard();

    PieceType getPieceTypeWithSetBitAtPosition(uint16_t position) const;

    static std::string getBitBoardString(const uint64_t&);

private:
    std::pair<uint64_t*, PieceFigure> getBoardWithSetBitAtPosition(uint16_t position,
                                                                   PieceColor color);
    std::vector<uint16_t>& getPiecePositions(const PieceType& type);

    bool parsePosition(const std::string& position);
    bool parseRow(const std::string& row, uint8_t rowIndex);
    bool parseActiveColor(const std::string& activeColor);
    bool parseCastlingRights(const std::string& castlingRights);
    bool parseEnPassant(const std::string& enPassant);

    void handleCastling(PieceFigure figure, Move move);
    void handleEnPassant(Move move);
    void handlePromotion(Move move);
};

inline void PieceBitBoards::setBit(uint64_t& number, uint16_t index)
{
    number |= (1ULL << index);
}

inline bool PieceBitBoards::getBit(uint64_t number, uint16_t index)
{
    return number & (1ULL << index);
}

inline void PieceBitBoards::clearBit(uint64_t& number, uint16_t index)
{
    (number) &= ~(1ULL << index);
}

inline std::vector<uint16_t> PieceBitBoards::getSetBitPositions(uint64_t number)
{
    std::vector<uint16_t> positions;
    for (uint16_t i = 0; i < 64; ++i) {
        if (number & (1ULL << i)) {
            positions.push_back(i);
        }
    }
    return positions;
}

inline uint16_t PieceBitBoards::countSetBits(uint64_t number)
{
    uint16_t count = 0;
    while (number) {
        number &= (number - 1);
        count++;
    }
    return count;
}

inline std::map<PieceType, const uint64_t*> PieceBitBoards::getTypeToPieceBitBoards() const
{
    return {
        std::make_pair(PieceType(PieceColor::White, PieceFigure::Pawn), &whitePawns),
        std::make_pair(PieceType(PieceColor::White, PieceFigure::Bishop), &whiteBishops),
        std::make_pair(PieceType(PieceColor::White, PieceFigure::Knight), &whiteKnights),
        std::make_pair(PieceType(PieceColor::White, PieceFigure::Rook), &whiteRooks),
        std::make_pair(PieceType(PieceColor::White, PieceFigure::Queen), &whiteQueens),
        std::make_pair(PieceType(PieceColor::White, PieceFigure::King), &whiteKing),
        std::make_pair(PieceType(PieceColor::Black, PieceFigure::Pawn), &blackPawns),
        std::make_pair(PieceType(PieceColor::Black, PieceFigure::Bishop), &blackBishops),
        std::make_pair(PieceType(PieceColor::Black, PieceFigure::Knight), &blackKnights),
        std::make_pair(PieceType(PieceColor::Black, PieceFigure::Rook), &blackRooks),
        std::make_pair(PieceType(PieceColor::Black, PieceFigure::Queen), &blackQueens),
        std::make_pair(PieceType(PieceColor::Black, PieceFigure::King), &blackKing),
    };
}

template <typename TBitWiseOperator>
inline PieceType PieceBitBoards::modifyAllBitBoards(uint64_t mask, TBitWiseOperator bitWiseOperator)
{
    auto tempWhitePawns = bitWiseOperator(whitePawns, mask);
    if (tempWhitePawns != whitePawns) {
        whitePawns = tempWhitePawns;
        return PieceType(PieceColor::White, PieceFigure::Pawn);
    }
    auto tempWhiteBishops = bitWiseOperator(whiteBishops, mask);
    if (tempWhiteBishops != whiteBishops) {
        whiteBishops = tempWhiteBishops;
        return PieceType(PieceColor::White, PieceFigure::Bishop);
    }
    auto tempWhiteKnights = bitWiseOperator(whiteKnights, mask);
    if (tempWhiteKnights != whiteKnights) {
        whiteKnights = tempWhiteKnights;
        return PieceType(PieceColor::White, PieceFigure::Knight);
    }
    auto tempWhiteRooks = bitWiseOperator(whiteRooks, mask);
    if (tempWhiteRooks != whiteRooks) {
        whiteRooks = tempWhiteRooks;
        return PieceType(PieceColor::White, PieceFigure::Rook);
    }
    auto tempWhiteQueens = bitWiseOperator(whiteQueens, mask);
    if (tempWhiteQueens != whiteQueens) {
        whiteQueens = tempWhiteQueens;
        return PieceType(PieceColor::White, PieceFigure::Queen);
    }
    auto tempWhiteKing = bitWiseOperator(whiteKing, mask);
    if (tempWhiteKing != whiteKing) {
        whiteKing = tempWhiteKing;
        return PieceType(PieceColor::White, PieceFigure::King);
    }

    auto tempBlackPawns = bitWiseOperator(blackPawns, mask);
    if (tempBlackPawns != blackPawns) {
        blackPawns = tempBlackPawns;
        return PieceType(PieceColor::Black, PieceFigure::Pawn);
    }
    auto tempBlackBishops = bitWiseOperator(blackBishops, mask);
    if (tempBlackBishops != blackBishops) {
        blackBishops = tempBlackBishops;
        return PieceType(PieceColor::Black, PieceFigure::Bishop);
    }
    auto tempBlackKnights = bitWiseOperator(blackKnights, mask);
    if (tempBlackKnights != blackKnights) {
        blackKnights = tempBlackKnights;
        return PieceType(PieceColor::Black, PieceFigure::Knight);
    }
    auto tempBlackRooks = bitWiseOperator(blackRooks, mask);
    if (tempBlackRooks != blackRooks) {
        blackRooks = tempBlackRooks;
        return PieceType(PieceColor::Black, PieceFigure::Rook);
    }
    auto tempBlackQueens = bitWiseOperator(blackQueens, mask);
    if (tempBlackQueens != blackQueens) {
        blackQueens = tempBlackQueens;
        return PieceType(PieceColor::Black, PieceFigure::Queen);
    }
    auto tempBlackKing = bitWiseOperator(blackKing, mask);
    if (tempBlackKing != blackKing) {
        blackKing = tempBlackKing;
        return PieceType(PieceColor::Black, PieceFigure::King);
    }
    return PieceType(PieceColor::White, PieceFigure::Empty);
}

inline uint64_t PieceBitBoards::getAllPiecesBoard() const
{
    return whitePawns | whiteBishops | whiteKnights | whiteRooks | whiteQueens | whiteKing |
           blackPawns | blackBishops | blackKnights | blackRooks | blackQueens | blackKing;
}

template <>
inline uint64_t PieceBitBoards::getAllPiecesBoard<PieceColor::White>() const
{
    return whitePawns | whiteBishops | whiteKnights | whiteRooks | whiteQueens | whiteKing;
}

template <>
inline uint64_t PieceBitBoards::getAllPiecesBoard<PieceColor::Black>() const
{
    return blackPawns | blackBishops | blackKnights | blackRooks | blackQueens | blackKing;
}

template <>
inline uint64_t PieceBitBoards::getAllOppositeColorPieces<PieceColor::White>() const
{
    return blackPawns | blackBishops | blackKnights | blackRooks | blackQueens | blackKing;
}

template <>
inline uint64_t PieceBitBoards::getAllOppositeColorPieces<PieceColor::Black>() const
{
    return whitePawns | whiteBishops | whiteKnights | whiteRooks | whiteQueens | whiteKing;
}

template <PieceColor TColor, PieceFigure TFigure>
inline constexpr uint64_t PieceBitBoards::getPieceBitBoard() const
{
    if constexpr (TColor == PieceColor::White) {
        if constexpr (TFigure == PieceFigure::Pawn)
            return whitePawns;
        if constexpr (TFigure == PieceFigure::Bishop)
            return whiteBishops;
        if constexpr (TFigure == PieceFigure::Rook)
            return whiteRooks;
        if constexpr (TFigure == PieceFigure::Knight)
            return whiteKnights;
        if constexpr (TFigure == PieceFigure::Queen)
            return whiteQueens;
        if constexpr (TFigure == PieceFigure::King)
            return whiteKing;
    }
    else {
        if constexpr (TFigure == PieceFigure::Pawn)
            return blackPawns;
        if constexpr (TFigure == PieceFigure::Bishop)
            return blackBishops;
        if constexpr (TFigure == PieceFigure::Rook)
            return blackRooks;
        if constexpr (TFigure == PieceFigure::Knight)
            return blackKnights;
        if constexpr (TFigure == PieceFigure::Queen)
            return blackQueens;
        if constexpr (TFigure == PieceFigure::King)
            return blackKing;
    }
}

template <PieceColor TColor, PieceFigure TFigure>
inline uint64_t& PieceBitBoards::getModifiablePieceBitBoard()
{
    if constexpr (TColor == PieceColor::White) {
        if constexpr (TFigure == PieceFigure::Pawn)
            return whitePawns;
        if constexpr (TFigure == PieceFigure::Bishop)
            return whiteBishops;
        if constexpr (TFigure == PieceFigure::Rook)
            return whiteRooks;
        if constexpr (TFigure == PieceFigure::Knight)
            return whiteKnights;
        if constexpr (TFigure == PieceFigure::Queen)
            return whiteQueens;
        if constexpr (TFigure == PieceFigure::King)
            return whiteKing;
    }
    else {
        if constexpr (TFigure == PieceFigure::Pawn)
            return blackPawns;
        if constexpr (TFigure == PieceFigure::Bishop)
            return blackBishops;
        if constexpr (TFigure == PieceFigure::Rook)
            return blackRooks;
        if constexpr (TFigure == PieceFigure::Knight)
            return blackKnights;
        if constexpr (TFigure == PieceFigure::Queen)
            return blackQueens;
        if constexpr (TFigure == PieceFigure::King)
            return blackKing;
    }
}

} // namespace chessAi