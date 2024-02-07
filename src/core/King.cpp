#include "King.h"
#include "PieceBitBoards.h"

#include <array>

namespace chessAi
{

uint64_t King::generateOneAttack(uint16_t origin)
{
    uint64_t attack = 0;
    if (origin / 8 < 7)
        PieceBitBoards::setBit(attack, origin + 8);
    if (origin / 8 > 0)
        PieceBitBoards::setBit(attack, origin - 8);
    if (origin % 8 > 0) {
        if (origin / 8 < 7)
            PieceBitBoards::setBit(attack, origin + 7);
        if (origin / 8 > 0)
            PieceBitBoards::setBit(attack, origin - 9);
        PieceBitBoards::setBit(attack, origin - 1);
    }
    if (origin % 8 < 7) {
        if (origin / 8 < 7)
            PieceBitBoards::setBit(attack, origin + 9);
        if (origin / 8 > 0)
            PieceBitBoards::setBit(attack, origin - 7);
        PieceBitBoards::setBit(attack, origin + 1);
    }
    return attack;
}

std::array<uint64_t, 64> King::generateAttacks()
{
    std::array<uint64_t, 64> attacks{};
    for (uint16_t i = 0; i < 64; ++i) {
        attacks[i] = generateOneAttack(i);
    }
    return attacks;
}

} // namespace chessAi