/**
 * Run tests in release mode, otherwise takes a lot of time.
 *
 */

#include <gtest/gtest.h>

#include "core/Evaluate.h"

namespace chessAi
{

TEST(Evaluation, StartingPositionWithoutPieces)
{
    EXPECT_EQ(Evaluate::getEvaluation(PieceBitBoards()), 0);
    // No pawns
    EXPECT_EQ(Evaluate::getEvaluation(
                  PieceBitBoards("rnbqkbnr/11pppppp/8/8/8/8/11PPPPPP/RNBQKBNR w KQkq - 0 1")),
              0);
    // No knights
    EXPECT_EQ(Evaluate::getEvaluation(
                  PieceBitBoards("rnbqkb1r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKB1R w KQkq - 0 1")),
              0);
    // No bishops
    EXPECT_EQ(Evaluate::getEvaluation(
                  PieceBitBoards("rnbqk1nr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK1NR w KQkq - 0 1")),
              0);
    // No queens
    EXPECT_EQ(Evaluate::getEvaluation(
                  PieceBitBoards("rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1")),
              0);
    // No rooks
    EXPECT_EQ(Evaluate::getEvaluation(
                  PieceBitBoards("1nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR w Kk - 0 1")),
              0);
}

} // namespace chessAi
