#include "BoardState.h"
#include "EndOfGameChecker.h"
#include "MoveGenerator.h"

#include <functional>

namespace chessAi
{

const PieceBitBoards& BoardState::getBitBoards() const
{
    return m_bitBoards;
}

PieceType BoardState::getPiece(unsigned int position) const
{
    for (const auto& [pieceType, bitBoard] : m_bitBoards.getTypeToPieceBitBoards()) {
        if (PieceBitBoards::getBit(*bitBoard, position) == 1) {
            return pieceType;
        }
    }
    return {PieceColor::White, PieceFigure::Empty};
}

namespace
{

template <PieceColor TColor, PieceFigure TFigure>
bool updateIfLegalMove(PieceBitBoards& bitBoards, Move move)
{
    // isLegalMove returns move with special flags set.
    auto legalMove = MoveGenerator<TColor, TFigure>::isLegalMove(bitBoards, move);
    if (!legalMove.first) {
        CHESS_LOG_TRACE("Move is illegal.");
        return false;
    }
    bitBoards.applyMove(legalMove.second);
    return true;
}

} // namespace

[[nodiscard]] EndOfGameType BoardState::updateBoardState(Move move)
{
    if (move.origin == move.destination) {
        CHESS_LOG_TRACE("Origin and destination are the same.");
        return EndOfGameType::None;
    }

    auto movingPiece = getPiece(move.origin);

    if (movingPiece.getPieceFigure() == PieceFigure::Empty) {
        CHESS_LOG_DEBUG("Updating board state with empty piece position.");
        return EndOfGameType::None;
    }
    if (movingPiece.getPieceColor() != m_bitBoards.currentMoveColor) {
        CHESS_LOG_TRACE("Wrong color piece to move.");
        return EndOfGameType::None;
    }

    // Move that is constructed here does not have special flags set.
    // Use isLegal move to get a returned move with special flags set for en passant, castling...
    switch (movingPiece.getPieceColor()) {
    case PieceColor::White:
        switch (movingPiece.getPieceFigure()) {
        case PieceFigure::Pawn:
            if (!updateIfLegalMove<PieceColor::White, PieceFigure::Pawn>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Bishop:
            if (!updateIfLegalMove<PieceColor::White, PieceFigure::Bishop>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Rook:
            if (!updateIfLegalMove<PieceColor::White, PieceFigure::Rook>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Knight:
            if (!updateIfLegalMove<PieceColor::White, PieceFigure::Knight>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::King:
            if (!updateIfLegalMove<PieceColor::White, PieceFigure::King>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Queen:
            if (!updateIfLegalMove<PieceColor::White, PieceFigure::Queen>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        default:
            CHESS_LOG_ERROR("Unhandled piece figure.");
            break;
        }
        break;
    case PieceColor::Black:
        switch (movingPiece.getPieceFigure()) {
        case PieceFigure::Pawn:
            if (!updateIfLegalMove<PieceColor::Black, PieceFigure::Pawn>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Bishop:
            if (!updateIfLegalMove<PieceColor::Black, PieceFigure::Bishop>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Rook:
            if (!updateIfLegalMove<PieceColor::Black, PieceFigure::Rook>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Knight:
            if (!updateIfLegalMove<PieceColor::Black, PieceFigure::Knight>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::King:
            if (!updateIfLegalMove<PieceColor::Black, PieceFigure::King>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        case PieceFigure::Queen:
            if (!updateIfLegalMove<PieceColor::Black, PieceFigure::Queen>(m_bitBoards, move)) {
                return EndOfGameType::None;
            }
            break;
        default:
            CHESS_LOG_ERROR("Unhandled piece figure.");
            break;
        }
        break;
    default:
        CHESS_LOG_ERROR("Unhandled piece color.");
        break;
    }
    m_bitBoardsHistory.push_back(m_bitBoards);
    m_movesHistory.push_back(std::move(move));

    return EndOfGameChecker::checkBoardState(m_bitBoards);
}

void BoardState::goToPreviousBoardState()
{
    if (m_bitBoardsHistory.size() <= 1) {
        return;
    }
    m_bitBoardsHistory.pop_back();
    m_bitBoards = m_bitBoardsHistory.back();
    if (m_movesHistory.empty()) {
        CHESS_LOG_ERROR("Moves history is empty, but should have at least one move.");
        return;
    }
    m_movesHistory.pop_back();
}

std::optional<Move> BoardState::getLastMove() const
{
    if (m_movesHistory.empty())
        return std::nullopt;
    return m_movesHistory.back();
}

BoardState::BoardState(const std::string& fenString)
    : m_bitBoards(), m_bitBoardsHistory({m_bitBoards}), m_movesHistory()
{
    if (!fenString.empty()) {
        m_bitBoards = PieceBitBoards(fenString);
        m_bitBoardsHistory = {m_bitBoards};
    }

    CHESS_LOG_TRACE("whitePawns: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.whitePawns));
    CHESS_LOG_TRACE("whiteBishops: \n{}",
                    PieceBitBoards::getBitBoardString(m_bitBoards.whiteBishops));
    CHESS_LOG_TRACE("whiteKnights: \n{}",
                    PieceBitBoards::getBitBoardString(m_bitBoards.whiteKnights));
    CHESS_LOG_TRACE("whiteRooks: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.whiteRooks));
    CHESS_LOG_TRACE("whiteQueen: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.whiteQueens));
    CHESS_LOG_TRACE("whiteKing: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.whiteKing));
    CHESS_LOG_TRACE("blackPawns: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.blackPawns));
    CHESS_LOG_TRACE("blackBishops: \n{}",
                    PieceBitBoards::getBitBoardString(m_bitBoards.blackBishops));
    CHESS_LOG_TRACE("blackKnights: \n{}",
                    PieceBitBoards::getBitBoardString(m_bitBoards.blackKnights));
    CHESS_LOG_TRACE("blackRooks: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.blackRooks));
    CHESS_LOG_TRACE("blackQueen: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.blackQueens));
    CHESS_LOG_TRACE("blackKing: \n{}", PieceBitBoards::getBitBoardString(m_bitBoards.blackKing));
}

} // namespace chessAi