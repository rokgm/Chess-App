#pragma once

#include <cstdint>

namespace chessAi
{

struct Move
{
    Move(uint16_t origin, uint16_t destination, uint16_t promotion, uint16_t specialMoveFlag);
    /**
     * Positions 0 - 63
     */
    uint16_t origin : 6;
    /**
     * Positions 0 - 63
     */
    uint16_t destination : 6;
    /**
     * 0 to Knight, 1 to Bishop, 2 to Rook, 3 to Queen
     */
    uint16_t promotion : 2;
    /**
     * 0 none, 1 promotion, 2 en passant, 3 castling
     */
    uint16_t specialMoveFlag : 2;

    bool operator==(Move other) const;
};

/**
 * Use for unary comparison, as in std::find_if for example. Compares only origin and destination
 * of move. If promotion flag is set, compares also type of promotion.
 */
struct SpecialMoveCompare
{
    explicit SpecialMoveCompare(Move move);
    bool operator()(Move other);
    Move m_move;
};

} // namespace chessAi