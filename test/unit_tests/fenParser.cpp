#include <gtest/gtest.h>

#include "core/PieceBitBoards.h"

namespace chessAi
{

TEST(FenParserTest, BasicPositionTest)
{
    // FEN r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1

    PieceBitBoards board1;
    // Set position.
    // Castle rights are true by default.
    board1.currentMoveColor = PieceColor::White;
    board1.whitePawns = 0b0000000011100111000000000001000000001000000000000000000000000000;
    board1.whiteKnights = 0b0000000000000000000001000000000000010000000000000000000000000000;
    board1.whiteBishops = 0b11000000000000000000000000000000000000000000000000000;
    PieceBitBoards::setBit(board1.whiteQueens, 45);
    PieceBitBoards::clearBit(board1.whiteQueens, 59);
    board1.blackPawns = 0b100000000000001000000000010100000010110100000000;
    PieceBitBoards::setBit(board1.blackKnights, 17);
    PieceBitBoards::setBit(board1.blackKnights, 21);
    PieceBitBoards::clearBit(board1.blackKnights, 1);
    PieceBitBoards::clearBit(board1.blackKnights, 6);
    board1.blackBishops = 0;
    PieceBitBoards::setBit(board1.blackBishops, 16);
    PieceBitBoards::setBit(board1.blackBishops, 14);
    board1.blackQueens = 0;
    PieceBitBoards::setBit(board1.blackQueens, 12);

    PieceBitBoards board2("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    EXPECT_EQ(board1.whitePawns, board2.whitePawns);
    EXPECT_EQ(board1.whiteBishops, board2.whiteBishops);
    EXPECT_EQ(board1.whiteKnights, board2.whiteKnights);
    EXPECT_EQ(board1.whiteRooks, board2.whiteRooks);
    EXPECT_EQ(board1.whiteQueens, board2.whiteQueens);
    EXPECT_EQ(board1.whiteKing, board2.whiteKing);
    EXPECT_EQ(board1.blackPawns, board2.blackPawns);
    EXPECT_EQ(board1.blackBishops, board2.blackBishops);
    EXPECT_EQ(board1.blackKnights, board2.blackKnights);
    EXPECT_EQ(board1.blackRooks, board2.blackRooks);
    EXPECT_EQ(board1.blackQueens, board2.blackQueens);
    EXPECT_EQ(board1.blackKing, board2.blackKing);

    EXPECT_EQ(board1.currentMoveColor, board2.currentMoveColor);
}

TEST(FenParserTest, DifferentActiveColor)
{
    PieceBitBoards board1;
    // Set position.
    // Castle rights are true by default.
    board1.currentMoveColor = PieceColor::Black;
    board1.whitePawns = 0b0000000011100111000000000001000000001000000000000000000000000000;
    board1.whiteKnights = 0b0000000000000000000001000000000000010000000000000000000000000000;
    board1.whiteBishops = 0b11000000000000000000000000000000000000000000000000000;
    PieceBitBoards::setBit(board1.whiteQueens, 45);
    PieceBitBoards::clearBit(board1.whiteQueens, 59);
    board1.blackPawns = 0b100000000000001000000000010100000010110100000000;
    PieceBitBoards::setBit(board1.blackKnights, 17);
    PieceBitBoards::setBit(board1.blackKnights, 21);
    PieceBitBoards::clearBit(board1.blackKnights, 1);
    PieceBitBoards::clearBit(board1.blackKnights, 6);
    board1.blackBishops = 0;
    PieceBitBoards::setBit(board1.blackBishops, 16);
    PieceBitBoards::setBit(board1.blackBishops, 14);
    board1.blackQueens = 0;
    PieceBitBoards::setBit(board1.blackQueens, 12);

    PieceBitBoards board2("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1");

    EXPECT_EQ(board1.whitePawns, board2.whitePawns);
    EXPECT_EQ(board1.whiteBishops, board2.whiteBishops);
    EXPECT_EQ(board1.whiteKnights, board2.whiteKnights);
    EXPECT_EQ(board1.whiteRooks, board2.whiteRooks);
    EXPECT_EQ(board1.whiteQueens, board2.whiteQueens);
    EXPECT_EQ(board1.whiteKing, board2.whiteKing);
    EXPECT_EQ(board1.blackPawns, board2.blackPawns);
    EXPECT_EQ(board1.blackBishops, board2.blackBishops);
    EXPECT_EQ(board1.blackKnights, board2.blackKnights);
    EXPECT_EQ(board1.blackRooks, board2.blackRooks);
    EXPECT_EQ(board1.blackQueens, board2.blackQueens);
    EXPECT_EQ(board1.blackKing, board2.blackKing);

    EXPECT_EQ(board1.currentMoveColor, board2.currentMoveColor);
}

TEST(FenParserTest, CastlingRightsTest)
{
    PieceBitBoards board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b K - 0 1");
    EXPECT_TRUE(board.whiteKingSideCastle);
    EXPECT_FALSE(board.whiteQueenSideCastle);
    EXPECT_FALSE(board.blackKingSideCastle);
    EXPECT_FALSE(board.blackQueenSideCastle);

    board = PieceBitBoards("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b - - 0 1");
    EXPECT_FALSE(board.whiteKingSideCastle);
    EXPECT_FALSE(board.whiteQueenSideCastle);
    EXPECT_FALSE(board.blackKingSideCastle);
    EXPECT_FALSE(board.blackQueenSideCastle);

    board = PieceBitBoards("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b Q - 0 1");
    EXPECT_FALSE(board.whiteKingSideCastle);
    EXPECT_TRUE(board.whiteQueenSideCastle);
    EXPECT_FALSE(board.blackKingSideCastle);
    EXPECT_FALSE(board.blackQueenSideCastle);

    board = PieceBitBoards("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b k - 0 1");
    EXPECT_FALSE(board.whiteKingSideCastle);
    EXPECT_FALSE(board.whiteQueenSideCastle);
    EXPECT_TRUE(board.blackKingSideCastle);
    EXPECT_FALSE(board.blackQueenSideCastle);

    board = PieceBitBoards("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b q - 0 1");
    EXPECT_FALSE(board.whiteKingSideCastle);
    EXPECT_FALSE(board.whiteQueenSideCastle);
    EXPECT_FALSE(board.blackKingSideCastle);
    EXPECT_TRUE(board.blackQueenSideCastle);
}

TEST(FenParserTest, EnPassant)
{
    auto board = PieceBitBoards("rnbqkbnr/ppp1p1pp/5p2/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
    EXPECT_EQ(board.enPassantTargetSquare, 19);

    board = PieceBitBoards("rnbqkbnr/ppp1pppp/8/8/3pPP2/8/PPPP2PP/RNBQKBNR b KQkq e3 0 3");
    EXPECT_EQ(board.enPassantTargetSquare, 44);
}

} // namespace chessAi
