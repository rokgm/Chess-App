#pragma once

#include <array>

namespace chessAi
{

class Knight
{
private:
    template <typename T, T... origins>
    static constexpr std::array<uint64_t, 64> generateAttacks(std::integer_sequence<T, origins...>);

    template <uint16_t TOrigin>
    static constexpr uint64_t generateOneAttack();

public:
    /**
     * Knight attack mask for given position. Knight position (0-63) is index in the array.
     * See tests for examples.
     */
    inline static const std::array<uint64_t, 64> originToAttacks{
        generateAttacks(std::make_index_sequence<64>{})};
};

template <uint16_t TOrigin>
constexpr uint64_t Knight::generateOneAttack()
{
    // Centered for knight on 2 position in row.
    uint8_t row1 = 0b01010;
    uint8_t row2 = 0b10001;
    if constexpr (TOrigin % 8 < 2) {
        row1 = row1 >> (2 - (TOrigin % 8));
        row2 = row2 >> (2 - (TOrigin % 8));
    }
    else {
        row1 = row1 << ((TOrigin % 8) - 2);
        row2 = row2 << ((TOrigin % 8) - 2);
    }

    uint64_t attack1 = row1;
    uint64_t attack2 = row2;
    uint64_t attack = attack1 | (attack2 << 8) | (attack2 << 24) | (attack1 << 32);

    if constexpr (TOrigin / 8 < 2) {
        return attack >> ((2 - (TOrigin / 8)) * 8);
    }
    else {
        return attack << (((TOrigin / 8) - 2) * 8);
    }
}

template <typename T, T... origins>
constexpr std::array<uint64_t, 64> Knight::generateAttacks(std::integer_sequence<T, origins...>)
{
    constexpr auto attacks = std::array{Knight::generateOneAttack<origins>()...};
    return attacks;
}

} // namespace chessAi