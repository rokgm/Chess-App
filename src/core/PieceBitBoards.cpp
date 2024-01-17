#include "PieceBitBoards.h"
#include "ZobristHash.h"

namespace chessAi
{

namespace
{

std::vector<std::string> splitString(const std::string& input, char delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(delimiter, start);
    }

    tokens.push_back(input.substr(start, end));

    return tokens;
}

} // namespace

PieceBitBoards::PieceBitBoards(const std::string& fen)
{
    auto tokens = splitString(fen, ' ');

    if (tokens.size() == 6) {
        if (parsePosition(tokens[0]) && parseActiveColor(tokens[1]) &&
            parseCastlingRights(tokens[2]) && parseEnPassant(tokens[3])) {

            zobristKey = ZobristHash::calculateZobristKey(*this);
            return;
        }
    }
    CHESS_LOG_ERROR("Fen string must have 6 parts separated by spaces.");

    // Default starting position.
    whitePawns = 0xFF000000000000ULL;
    whiteBishops = 0x2400000000000000ULL;
    whiteKnights = 0x4200000000000000ULL;
    whiteRooks = 0x8100000000000000ULL;
    whiteQueens = 0x800000000000000ULL;
    whiteKing = 0x1000000000000000ULL;
    blackPawns = 0xff00ULL;
    blackBishops = 0x24ULL;
    blackKnights = 0x42ULL;
    blackRooks = 0x81ULL;
    blackQueens = 0x8ULL;
    blackKing = 0x10ULL;

    zobristKey = ZobristHash::calculateZobristKey(*this);
}

bool PieceBitBoards::parsePosition(const std::string& position)
{
    auto rows = splitString(position, '/');

    if (rows.size() != 8) {
        CHESS_LOG_ERROR("Position part of fen must have 8 parts separate by /.");
        return false;
    }

    uint8_t rowIndex = 0;
    for (const auto& row : rows) {
        if (!parseRow(row, rowIndex))
            return false;
        rowIndex++;
    }
    return true;
}

bool PieceBitBoards::parseRow(const std::string& row, uint8_t rowIndex)
{
    uint8_t columnIndex = 0;

    for (unsigned char ch : row) {
        if (std::isdigit(ch)) {
            uint8_t number = ch - '0';
            if (number < 1 || number > 8) {
                CHESS_LOG_ERROR("Fen number for piece placement must be between 0-8.");
                return false;
            }
            columnIndex += number;
        }
        else {
            switch (ch) {
            case 'P':
                PieceBitBoards::setBit(whitePawns, rowIndex * 8 + columnIndex);
                break;
            case 'N':
                PieceBitBoards::setBit(whiteKnights, rowIndex * 8 + columnIndex);
                break;
            case 'B':
                PieceBitBoards::setBit(whiteBishops, rowIndex * 8 + columnIndex);
                break;
            case 'R':
                PieceBitBoards::setBit(whiteRooks, rowIndex * 8 + columnIndex);
                break;
            case 'Q':
                PieceBitBoards::setBit(whiteQueens, rowIndex * 8 + columnIndex);
                break;
            case 'K':
                PieceBitBoards::setBit(whiteKing, rowIndex * 8 + columnIndex);
                break;
            case 'p':
                PieceBitBoards::setBit(blackPawns, rowIndex * 8 + columnIndex);
                break;
            case 'n':
                PieceBitBoards::setBit(blackKnights, rowIndex * 8 + columnIndex);
                break;
            case 'b':
                PieceBitBoards::setBit(blackBishops, rowIndex * 8 + columnIndex);
                break;
            case 'r':
                PieceBitBoards::setBit(blackRooks, rowIndex * 8 + columnIndex);
                break;
            case 'q':
                PieceBitBoards::setBit(blackQueens, rowIndex * 8 + columnIndex);
                break;
            case 'k':
                PieceBitBoards::setBit(blackKing, rowIndex * 8 + columnIndex);
                break;
            default:
                CHESS_LOG_ERROR("Fen piece placement character is illegal.");
                return false;
            }
            columnIndex++;
        }
    }
    return true;
}

bool PieceBitBoards::parseActiveColor(const std::string& activeColor)
{
    if (activeColor == "w")
        currentMoveColor = PieceColor::White;
    else if (activeColor == "b")
        currentMoveColor = PieceColor::Black;
    else {
        CHESS_LOG_ERROR("Wrong format of fen active color.");
        return false;
    }
    return true;
}

bool PieceBitBoards::parseCastlingRights(const std::string& castlingRights)
{
    whiteKingSideCastle = false;
    whiteQueenSideCastle = false;
    blackKingSideCastle = false;
    blackQueenSideCastle = false;

    if (castlingRights == "-")
        return true;

    for (char ch : castlingRights) {
        if (ch == 'K')
            whiteKingSideCastle = true;
        else if (ch == 'Q')
            whiteQueenSideCastle = true;
        else if (ch == 'k')
            blackKingSideCastle = true;
        else if (ch == 'q')
            blackQueenSideCastle = true;
        else {
            CHESS_LOG_ERROR("Fen, wrong castling rights format.");
            return false;
        }
    }
    return true;
}

bool PieceBitBoards::parseEnPassant(const std::string& enPassant)
{
    if (enPassant == "-")
        return true;

    if (enPassant.length() != 2) {
        CHESS_LOG_ERROR("Fen: en passant algebraic notation error.");
        return false;
    }

    int x = enPassant[0] - 97;
    if (x < 0 || x > 7) {
        CHESS_LOG_ERROR("Fen: en passant algebraic notation error: file.");
        return false;
    }
    if (!std::isdigit(enPassant[1])) {
        CHESS_LOG_ERROR("Fen: en passant algebraic notation error: rank.");
        return false;
    }

    int y = enPassant[1] - '0';
    y = 8 - y;
    if (y == 2 || y == 5) {
        enPassantTargetSquare = static_cast<uint16_t>(x + 8 * y);
    }
    else {
        CHESS_LOG_ERROR("Fen: en passant algebraic notation error: rank.");
        return false;
    }
    return true;
}

std::string PieceBitBoards::getBitBoardString(const uint64_t& bitBoard)
{
    std::string representation;

    for (unsigned int i = 0; i < 8; ++i) {
        for (unsigned int j = 0; j < 8; ++j) {
            representation.append(std::to_string(getBit(bitBoard, i * 8 + j)));
        }
        representation.append("\n");
    }
    return representation;
}

std::pair<uint64_t*, PieceFigure> PieceBitBoards::getBoardWithSetBitAtPosition(uint16_t position,
                                                                               PieceColor color)
{
    if (color == PieceColor::White) {
        if (getBit(whitePawns, position) == 1)
            return {&whitePawns, PieceFigure::Pawn};
        if (getBit(whiteBishops, position) == 1)
            return {&whiteBishops, PieceFigure::Bishop};
        if (getBit(whiteKnights, position) == 1)
            return {&whiteKnights, PieceFigure::Knight};
        if (getBit(whiteRooks, position) == 1)
            return {&whiteRooks, PieceFigure::Rook};
        if (getBit(whiteQueens, position) == 1)
            return {&whiteQueens, PieceFigure::Queen};
        if (getBit(whiteKing, position) == 1)
            return {&whiteKing, PieceFigure::King};
        return {nullptr, PieceFigure::Empty};
    }
    else {
        if (getBit(blackPawns, position) == 1)
            return {&blackPawns, PieceFigure::Pawn};
        if (getBit(blackBishops, position) == 1)
            return {&blackBishops, PieceFigure::Bishop};
        if (getBit(blackKnights, position) == 1)
            return {&blackKnights, PieceFigure::Knight};
        if (getBit(blackRooks, position) == 1)
            return {&blackRooks, PieceFigure::Rook};
        if (getBit(blackQueens, position) == 1)
            return {&blackQueens, PieceFigure::Queen};
        if (getBit(blackKing, position) == 1)
            return {&blackKing, PieceFigure::King};
        return {nullptr, PieceFigure::Empty};
    }
}

PieceType PieceBitBoards::getPieceTypeWithSetBitAtPosition(uint16_t position) const
{
    if (getBit(whitePawns, position) == 1)
        return PieceType(PieceColor::White, PieceFigure::Pawn);
    if (getBit(whiteBishops, position) == 1)
        return PieceType(PieceColor::White, PieceFigure::Bishop);
    if (getBit(whiteKnights, position) == 1)
        return PieceType(PieceColor::White, PieceFigure::Knight);
    if (getBit(whiteRooks, position) == 1)
        return PieceType(PieceColor::White, PieceFigure::Rook);
    if (getBit(whiteQueens, position) == 1)
        return PieceType(PieceColor::White, PieceFigure::Queen);
    if (getBit(whiteKing, position) == 1)
        return PieceType(PieceColor::White, PieceFigure::King);
    if (getBit(blackPawns, position) == 1)
        return PieceType(PieceColor::Black, PieceFigure::Pawn);
    if (getBit(blackBishops, position) == 1)
        return PieceType(PieceColor::Black, PieceFigure::Bishop);
    if (getBit(blackKnights, position) == 1)
        return PieceType(PieceColor::Black, PieceFigure::Knight);
    if (getBit(blackRooks, position) == 1)
        return PieceType(PieceColor::Black, PieceFigure::Rook);
    if (getBit(blackQueens, position) == 1)
        return PieceType(PieceColor::Black, PieceFigure::Queen);
    if (getBit(blackKing, position) == 1)
        return PieceType(PieceColor::Black, PieceFigure::King);
    return PieceType(PieceColor::White, PieceFigure::Empty);
}

void PieceBitBoards::applyMove(Move move)
{
    auto [figureBoard, figure] = getBoardWithSetBitAtPosition(move.origin, currentMoveColor);

    // Check for 2 square pawn push.
    // First undo hash of en passant square if set.
    if (enPassantTargetSquare != 0) {
        zobristKey ^= ZobristHash::getEnPassantFile()[enPassantTargetSquare % 8];
    }
    if (figure == PieceFigure::Pawn) {
        if (abs(static_cast<int16_t>(move.origin) - static_cast<int16_t>(move.destination)) == 16) {
            enPassantTargetSquare = static_cast<uint16_t>(move.origin + move.destination) / 2;
            zobristKey ^= ZobristHash::getEnPassantFile()[enPassantTargetSquare % 8];
        }
        else
            enPassantTargetSquare = 0;
    }
    else
        enPassantTargetSquare = 0;

    if (figure == PieceFigure::Empty) {
        CHESS_LOG_ERROR("Applying move with no piece at origin.");
        return;
    }

    // Set new position
    uint64_t newPositionMask = 0;
    PieceBitBoards::setBit(newPositionMask, move.destination);
    auto typeChanged = modifyAllBitBoards(~newPositionMask, std::bit_and());
    PieceBitBoards::setBit(*figureBoard, move.destination);
    PieceBitBoards::clearBit(*figureBoard, move.origin);

    // Update zobrist key
    auto pieceIndex = PieceType(currentMoveColor, figure).getPieceIndex();
    zobristKey ^= ZobristHash::getPieces()[move.origin][pieceIndex];
    zobristKey ^= ZobristHash::getPieces()[move.destination][pieceIndex];

    if (typeChanged.getPieceFigure() != PieceFigure::Empty)
        zobristKey ^= ZobristHash::getPieces()[move.destination][typeChanged.getPieceIndex()];

    // If king moves, castling privilege is lost.
    if (figure == PieceFigure::King) {
        if (currentMoveColor == PieceColor::White) {
            if (whiteKingSideCastle)
                zobristKey ^= ZobristHash::getCastlingRights()[0];
            if (whiteQueenSideCastle)
                zobristKey ^= ZobristHash::getCastlingRights()[1];
            whiteKingSideCastle = false;
            whiteQueenSideCastle = false;
        }
        else {
            if (blackKingSideCastle)
                zobristKey ^= ZobristHash::getCastlingRights()[2];
            if (blackQueenSideCastle)
                zobristKey ^= ZobristHash::getCastlingRights()[3];
            blackKingSideCastle = false;
            blackQueenSideCastle = false;
        }
    }
    // If rook moves from square or is captured, castling privilege is lost.
    if (!(whiteRooks & 0b1000000000000000000000000000000000000000000000000000000000000000)) {
        if (whiteKingSideCastle)
            zobristKey ^= ZobristHash::getCastlingRights()[0];
        whiteKingSideCastle = false;
    }
    if (!(whiteRooks & 0b100000000000000000000000000000000000000000000000000000000)) {
        if (whiteQueenSideCastle)
            zobristKey ^= ZobristHash::getCastlingRights()[1];
        whiteQueenSideCastle = false;
    }
    if (!(blackRooks & 0b10000000)) {
        if (blackKingSideCastle)
            zobristKey ^= ZobristHash::getCastlingRights()[2];
        blackKingSideCastle = false;
    }
    if (!(blackRooks & 0b1)) {
        if (blackQueenSideCastle)
            zobristKey ^= ZobristHash::getCastlingRights()[3];
        blackQueenSideCastle = false;
    }

    // En passant
    if (move.specialMoveFlag == 2) {
        if (currentMoveColor == PieceColor::White) {
            PieceBitBoards::clearBit(blackPawns, move.destination + 8);
            zobristKey ^= ZobristHash::getPieces()[move.destination + 8]
                                                  [PieceType(PieceColor::Black, PieceFigure::Pawn)
                                                       .getPieceIndex()];
        }
        else {
            PieceBitBoards::clearBit(whitePawns, move.destination - 8);
            zobristKey ^= ZobristHash::getPieces()[move.destination - 8]
                                                  [PieceType(PieceColor::White, PieceFigure::Pawn)
                                                       .getPieceIndex()];
        }
    }

    // Castling. Castling flags are already set to false with checks above (checked with: if king
    // moved).
    if (move.specialMoveFlag == 3) {
        if (currentMoveColor == PieceColor::White) {
            if (move.destination == 62) {
                PieceBitBoards::setBit(whiteRooks, 61);
                PieceBitBoards::clearBit(whiteRooks, 63);
                zobristKey ^=
                    ZobristHash::getPieces()[61][PieceType(PieceColor::White, PieceFigure::Rook)
                                                     .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[63][PieceType(PieceColor::White, PieceFigure::Rook)
                                                     .getPieceIndex()];
            }
            else if (move.destination == 58) {
                PieceBitBoards::setBit(whiteRooks, 59);
                PieceBitBoards::clearBit(whiteRooks, 56);
                zobristKey ^=
                    ZobristHash::getPieces()[59][PieceType(PieceColor::White, PieceFigure::Rook)
                                                     .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[56][PieceType(PieceColor::White, PieceFigure::Rook)
                                                     .getPieceIndex()];
            }
            else
                CHESS_LOG_ERROR("Castling king destination is wrong.");
        }
        else {
            if (move.destination == 6) {
                PieceBitBoards::setBit(blackRooks, 5);
                PieceBitBoards::clearBit(blackRooks, 7);
                zobristKey ^=
                    ZobristHash::getPieces()[5][PieceType(PieceColor::Black, PieceFigure::Rook)
                                                    .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[7][PieceType(PieceColor::Black, PieceFigure::Rook)
                                                    .getPieceIndex()];
            }
            else if (move.destination == 2) {
                PieceBitBoards::setBit(blackRooks, 3);
                PieceBitBoards::clearBit(blackRooks, 0);
                zobristKey ^=
                    ZobristHash::getPieces()[3][PieceType(PieceColor::Black, PieceFigure::Rook)
                                                    .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[0][PieceType(PieceColor::Black, PieceFigure::Rook)
                                                    .getPieceIndex()];
            }
            else
                CHESS_LOG_ERROR("Castling king destination is wrong.");
        }
    }

    // Promotion
    if (move.specialMoveFlag == 1) {
        if (currentMoveColor == PieceColor::White) {
            if (move.promotion == 0) {
                PieceBitBoards::setBit(whiteKnights, move.destination);
                PieceBitBoards::clearBit(whitePawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Knight)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else if (move.promotion == 1) {
                PieceBitBoards::setBit(whiteBishops, move.destination);
                PieceBitBoards::clearBit(whitePawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Bishop)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else if (move.promotion == 2) {
                PieceBitBoards::setBit(whiteRooks, move.destination);
                PieceBitBoards::clearBit(whitePawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Rook)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else if (move.promotion == 3) {
                PieceBitBoards::setBit(whiteQueens, move.destination);
                PieceBitBoards::clearBit(whitePawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Queen)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::White, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else
                CHESS_LOG_ERROR("Promotion type is invalid.");
        }
        else {
            if (move.promotion == 0) {
                PieceBitBoards::setBit(blackKnights, move.destination);
                PieceBitBoards::clearBit(blackPawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Knight)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else if (move.promotion == 1) {
                PieceBitBoards::setBit(blackBishops, move.destination);
                PieceBitBoards::clearBit(blackPawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Bishop)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else if (move.promotion == 2) {
                PieceBitBoards::setBit(blackRooks, move.destination);
                PieceBitBoards::clearBit(blackPawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Rook)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else if (move.promotion == 3) {
                PieceBitBoards::setBit(blackQueens, move.destination);
                PieceBitBoards::clearBit(blackPawns, move.destination);
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Queen)
                                                 .getPieceIndex()];
                zobristKey ^=
                    ZobristHash::getPieces()[move.destination]
                                            [PieceType(PieceColor::Black, PieceFigure::Pawn)
                                                 .getPieceIndex()];
            }
            else
                CHESS_LOG_ERROR("Promotion type is invalid.");
        }
    }
    currentMoveColor = PieceType::getOppositeColor(currentMoveColor);
    zobristKey ^= ZobristHash::getSideToMove();
    halfMoveCount++;
}

uint64_t PieceBitBoards::getPieceBitBoard(const PieceType& type) const
{
    if (type.getPieceColor() == PieceColor::White) {
        if (type.getPieceFigure() == PieceFigure::Pawn)
            return whitePawns;
        if (type.getPieceFigure() == PieceFigure::Bishop)
            return whiteBishops;
        if (type.getPieceFigure() == PieceFigure::Rook)
            return whiteRooks;
        if (type.getPieceFigure() == PieceFigure::Knight)
            return whiteKnights;
        if (type.getPieceFigure() == PieceFigure::Queen)
            return whiteQueens;
        if (type.getPieceFigure() == PieceFigure::King)
            return whiteKing;
    }
    else {
        if (type.getPieceFigure() == PieceFigure::Pawn)
            return blackPawns;
        if (type.getPieceFigure() == PieceFigure::Bishop)
            return blackBishops;
        if (type.getPieceFigure() == PieceFigure::Rook)
            return blackRooks;
        if (type.getPieceFigure() == PieceFigure::Knight)
            return blackKnights;
        if (type.getPieceFigure() == PieceFigure::Queen)
            return blackQueens;
        if (type.getPieceFigure() == PieceFigure::King)
            return blackKing;
    }
    return 0;
}

} // namespace chessAi