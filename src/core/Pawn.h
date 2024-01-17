#pragma once

#include "PieceType.h"

#include <array>

namespace chessAi
{

template <PieceColor TColor>
class Pawn
{
private:
    template <typename T, T... origins>
    static constexpr std::array<uint64_t, 64> generateAttacks(std::integer_sequence<T, origins...>);

    template <uint16_t TOrigin>
    static constexpr uint64_t generateOneAttack();

    template <typename T, T... origins>
    static constexpr std::array<uint64_t, 64> generatePushes(std::integer_sequence<T, origins...>);

    template <uint16_t TOrigin>
    static constexpr uint64_t generateOnePush();

    template <typename T, T... origins>
    static constexpr std::array<uint64_t, 64> generateTwoPushes(
        std::integer_sequence<T, origins...>);

    template <uint16_t TOrigin>
    static constexpr uint64_t generateOneTwoPush();

    template <typename T, T... origins>
    static constexpr std::array<uint64_t, 64> generateEnPassant(
        std::integer_sequence<T, origins...>);

    template <uint16_t TOrigin>
    static constexpr uint64_t generateOneEnPassant();

public:
    /**
     * Pawn attack mask for given position. Pawn position (0-63) is index in the array.
     * See tests for examples.
     */
    static constexpr std::array<uint64_t, 64> originToAttacks =
        generateAttacks(std::make_index_sequence<64>{});

    /**
     * Pawn one square push mask for given position. Pawn position (0-63) is index  is index in the
     * array.
     * See tests for examples.
     */
    static constexpr std::array<uint64_t, 64> originToPushes =
        generatePushes(std::make_index_sequence<64>{});

    /**
     * Pawn two squares push mask for given position. Pawn position (0-63) is index  is index in the
     * array.
     * See tests for examples.
     */
    static constexpr std::array<uint64_t, 64> originToTwoPushes =
        generateTwoPushes(std::make_index_sequence<64>{});

    /**
     * Get square mask (1 set at square position) which is jumper over when doing two square push.
     * See tests for examples.
     */
    static uint64_t getFieldJumpedOverWithTwoPush(uint16_t origin);

    /**
     * Mask of en passant taken positions. Left and right of current pawn.
     * See tests for examples.
     */
    static constexpr std::array<uint64_t, 64> originToEnPassant =
        generateEnPassant(std::make_index_sequence<64>{});
};

template <PieceColor TColor>
template <uint16_t TOrigin>
constexpr uint64_t Pawn<TColor>::generateOneAttack()
{
    uint8_t row = 0b101;
    if constexpr (TOrigin % 8 == 0) {
        row = row >> 1;
    }
    else {
        row = row << ((TOrigin % 8) - 1);
    }

    uint64_t attack = static_cast<uint64_t>(row);
    if constexpr (TColor == PieceColor::Black) {
        if constexpr (TOrigin > 55) {
            return 0;
        }
        else {
            return attack << ((TOrigin / 8) * 8 + 8);
        }
    }
    else if constexpr (TColor == PieceColor::White) {
        if constexpr (TOrigin < 8) {
            return 0;
        }
        else {
            return attack << ((TOrigin / 8) * 8 - 8);
        }
    }
}

template <PieceColor TColor>
template <typename T, T... origins>
constexpr std::array<uint64_t, 64> Pawn<TColor>::generateAttacks(
    std::integer_sequence<T, origins...>)
{
    constexpr auto attacks = std::array{Pawn<TColor>::generateOneAttack<origins>()...};
    return attacks;
}

template <PieceColor TColor>
template <uint16_t TOrigin>
constexpr uint64_t Pawn<TColor>::generateOnePush()
{
    if constexpr (TColor == PieceColor::Black) {
        if constexpr (TOrigin > 55) {
            return 0;
        }
        else {
            return 1ULL << (TOrigin + 8);
        }
    }
    else if constexpr (TColor == PieceColor::White) {
        if constexpr (TOrigin < 8) {
            return 0;
        }
        else {
            return 1ULL << (TOrigin - 8);
        }
    }
}

template <PieceColor TColor>
template <typename T, T... origins>
constexpr std::array<uint64_t, 64> Pawn<TColor>::generatePushes(
    std::integer_sequence<T, origins...>)
{
    constexpr auto attacks = std::array{Pawn<TColor>::generateOnePush<origins>()...};
    return attacks;
}

template <PieceColor TColor>
template <uint16_t TOrigin>
constexpr uint64_t Pawn<TColor>::generateOneTwoPush()
{
    if constexpr (TColor == PieceColor::Black) {
        if constexpr (TOrigin / 8 == 1) {
            return 1ULL << (TOrigin + 16);
        }
        else {
            return 0;
        }
    }
    else if constexpr (TColor == PieceColor::White) {
        if constexpr (TOrigin / 8 == 6) {
            return 1ULL << (TOrigin - 16);
        }
        else {
            return 0;
        }
    }
}

template <PieceColor TColor>
template <typename T, T... origins>
constexpr std::array<uint64_t, 64> Pawn<TColor>::generateTwoPushes(
    std::integer_sequence<T, origins...>)
{
    constexpr auto attacks = std::array{Pawn<TColor>::generateOneTwoPush<origins>()...};
    return attacks;
}

template <PieceColor TColor>
template <uint16_t TOrigin>
constexpr uint64_t Pawn<TColor>::generateOneEnPassant()
{
    uint8_t row = 0b101;
    if constexpr (TOrigin % 8 == 0) {
        row = row >> 1;
    }
    else {
        row = row << ((TOrigin % 8) - 1);
    }

    uint64_t enPassant = static_cast<uint64_t>(row);
    if constexpr (TColor == PieceColor::Black) {
        if constexpr (TOrigin / 8 != 4) {
            return 0;
        }
        else {
            return enPassant << ((TOrigin / 8) * 8);
        }
    }
    else if constexpr (TColor == PieceColor::White) {
        if constexpr (TOrigin / 8 != 3) {
            return 0;
        }
        else {
            return enPassant << ((TOrigin / 8) * 8);
        }
    }
}

template <PieceColor TColor>
template <typename T, T... origins>
constexpr std::array<uint64_t, 64> Pawn<TColor>::generateEnPassant(
    std::integer_sequence<T, origins...>)
{
    constexpr auto attacks = std::array{Pawn<TColor>::generateOneEnPassant<origins>()...};
    return attacks;
}

template <PieceColor TColor>
uint64_t Pawn<TColor>::getFieldJumpedOverWithTwoPush(uint16_t origin)
{
    if constexpr (TColor == PieceColor::Black) {
        if (origin / 8 == 1) {
            return 1ULL << (origin + 8);
        }
        return 0;
    }
    else if constexpr (TColor == PieceColor::White) {
        if (origin / 8 == 6) {
            return 1ULL << (origin - 8);
        }
        return 0;
    }
    else {
        return 0;
    }
}

} // namespace chessAi