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
        if (PieceBitBoards::getBit(*bitBoard, static_cast<uint16_t>(position)) == 1) {
            return pieceType;
        }
    }
    return {PieceColor::White, PieceFigure::Empty};
}

namespace
{

template <PieceColor TColor>
std::optional<Move> updateIfLegalMove(PieceBitBoards& bitBoards, Move move, PieceFigure figure)
{
    // isLegalMove returns move with special flags set.
    auto legalMove = MoveGenerator<TColor>::isLegalMove(bitBoards, move, figure);
    if (!legalMove.first) {
        CHESS_LOG_TRACE("Move is illegal.");
        return std::nullopt;
    }
    bitBoards.applyMove(legalMove.second);
    return legalMove.second;
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

    // Move here does not have special flags set.
    // Use isLegal move to get a returned move with special flags set for en passant, castling...
    std::optional<Move> appliedMove;
    switch (movingPiece.getPieceColor()) {
    case PieceColor::White:
        appliedMove =
            updateIfLegalMove<PieceColor::White>(m_bitBoards, move, movingPiece.getPieceFigure());
        if (!appliedMove.has_value()) {
            return EndOfGameType::None;
        }
        break;
    case PieceColor::Black:
        appliedMove =
            updateIfLegalMove<PieceColor::Black>(m_bitBoards, move, movingPiece.getPieceFigure());
        if (!appliedMove.has_value()) {
            return EndOfGameType::None;
        }
        break;
    default:
        CHESS_LOG_ERROR("Unhandled piece color.");
        break;
    }
    m_bitBoardsHistory.push_back(m_bitBoards);
    m_movesHistory.push_back(std::move(appliedMove.value()));

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

const std::vector<Move>& BoardState::getMovesHistory() const
{
    return m_movesHistory;
}

const std::vector<PieceBitBoards>& BoardState::getBitBoardsHistory() const
{
    return m_bitBoardsHistory;
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

std::vector<uint64_t> BoardState::getZobristKeyHistory() const
{
    std::vector<uint64_t> history;
    for (const auto& board : m_bitBoardsHistory) {
        history.push_back(board.zobristKey);
    }
    return history;
}

} // namespace chessAi