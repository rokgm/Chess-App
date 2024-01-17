#include <gtest/gtest.h>

#include "core/Pawn.h"

namespace chessAi
{

TEST(Attacks, originToAttacks)
{
    EXPECT_EQ(Pawn<PieceColor::Black>::originToAttacks[0], 0b1000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::White>::originToAttacks[63],
              0b01000000000000000000000000000000000000000000000000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::Black>::originToAttacks[8], 0b100000000000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::White>::originToAttacks[55],
              0b010000000000000000000000000000000000000000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::Black>::originToAttacks[15], 0b10000000000000000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::Black>::originToAttacks[1], 0b10100000000ULL);
    EXPECT_EQ(Pawn<PieceColor::White>::originToAttacks[62],
              0b10100000000000000000000000000000000000000000000000000000ULL);
}

TEST(Pushes, originToPushes)
{
    EXPECT_EQ(Pawn<PieceColor::Black>::originToPushes[0], 0b100000000ULL);
    EXPECT_EQ(Pawn<PieceColor::White>::originToPushes[63],
              0b10000000000000000000000000000000000000000000000000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::Black>::originToPushes[63], 0ULL);
    EXPECT_EQ(Pawn<PieceColor::White>::originToPushes[0], 0ULL);
}

TEST(TwoSquaresPushes, originToPushes)
{
    EXPECT_EQ(Pawn<PieceColor::Black>::originToTwoPushes[8], 0b1000000000000000000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::White>::originToTwoPushes[55],
              0b1000000000000000000000000000000000000000ULL);
    for (size_t i = 0; i < 64; ++i) {
        if (i / 8 != 1) {
            EXPECT_EQ(Pawn<PieceColor::Black>::originToTwoPushes[i], 0);
        }
        else if (i / 8 != 6) {
            EXPECT_EQ(Pawn<PieceColor::White>::originToTwoPushes[i], 0);
        }
    }
}

TEST(TwoSquaresPushes, jumpedOverSquareMask)
{
    EXPECT_EQ(Pawn<PieceColor::Black>::getFieldJumpedOverWithTwoPush(8), 0b10000000000000000ULL);
    EXPECT_EQ(Pawn<PieceColor::White>::getFieldJumpedOverWithTwoPush(55),
              0b100000000000000000000000000000000000000000000000ULL);
    for (uint16_t i = 0; i < 64; ++i) {
        if (i / 8 != 1) {
            EXPECT_EQ(Pawn<PieceColor::Black>::getFieldJumpedOverWithTwoPush(i), 0);
        }
        else if (i / 8 != 6) {
            EXPECT_EQ(Pawn<PieceColor::White>::getFieldJumpedOverWithTwoPush(i), 0);
        }
    }
}

TEST(EnPassant, takenPositions)
{
    EXPECT_EQ(Pawn<PieceColor::Black>::originToEnPassant[32], 0b1000000000000000000000000000000000);
    EXPECT_EQ(Pawn<PieceColor::Black>::originToEnPassant[35],
              0b1010000000000000000000000000000000000);
    EXPECT_EQ(Pawn<PieceColor::Black>::originToEnPassant[39],
              0b100000000000000000000000000000000000000);
    EXPECT_EQ(Pawn<PieceColor::White>::originToEnPassant[24], 0b10000000000000000000000000);
    EXPECT_EQ(Pawn<PieceColor::White>::originToEnPassant[27], 0b10100000000000000000000000000);
    EXPECT_EQ(Pawn<PieceColor::White>::originToEnPassant[31], 0b1000000000000000000000000000000);
    for (uint16_t i = 0; i < 64; ++i) {
        if (i / 8 != 4) {
            EXPECT_EQ(Pawn<PieceColor::Black>::originToEnPassant[i], 0);
        }
        else if (i / 8 != 3) {
            EXPECT_EQ(Pawn<PieceColor::White>::originToEnPassant[i], 0);
        }
    }
}

} // namespace chessAi
