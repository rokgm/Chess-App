#include <gtest/gtest.h>

#include "core/Knight.h"

namespace chessAi
{

TEST(KnightAttacks, originToAttacks)
{
    EXPECT_EQ(Knight::originToAttacks[18], 0b101000010001000000000001000100001010);
}

} // namespace chessAi
