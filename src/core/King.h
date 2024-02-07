#pragma once

#include <array>

namespace chessAi
{

class King
{
private:
    static std::array<uint64_t, 64> generateAttacks();

    static uint64_t generateOneAttack(uint16_t origin);

public:
    /**
     * Knight attack mask for given position. Knight position (0-63) is index in the array.
     * See tests for examples.
     */
    inline static const std::array<uint64_t, 64> originToAttacks{generateAttacks()};

    /**
     * Mask for squares that no piece and no attack must be on.
     */
    inline static uint64_t blackKingSideCastleMask = 0b01100000;
    /**
     * Mask for squares that no piece and no attack must be on.
     */
    inline static uint64_t whiteKingSideCastleMask =
        0b0110000000000000000000000000000000000000000000000000000000000000;
    /**
     * Mask for squares that no attack must be on.
     */
    inline static uint64_t blackQueenSideCastleAttackedMask = 0b1100;
    /**
     * Mask for squares that no piece must be on.
     */
    inline static uint64_t blackQueenSideCastlePiecesMask = 0b1110;
    /**
     * Mask for squares that no attack must be on.
     */
    inline static uint64_t whiteQueenSideCastleAttackedMask =
        0b110000000000000000000000000000000000000000000000000000000000;
    /**
     * Mask for squares that no piece must be on.
     */
    inline static uint64_t whiteQueenSideCastlePiecesMask =
        0b111000000000000000000000000000000000000000000000000000000000;
};

} // namespace chessAi