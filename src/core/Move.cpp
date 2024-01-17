#include "Move.h"

namespace chessAi
{

#if __GNUC__
    #pragma GCC diagnostic ignored "-Wconversion"
#endif
Move::Move(uint16_t origin, uint16_t destination, uint16_t promotion, uint16_t specialMoveFlag)
    : origin(origin), destination(destination), promotion(promotion),
      specialMoveFlag(specialMoveFlag)
{
}
#if __GNUC__
    #pragma GCC diagnostic pop
#endif

bool Move::operator==(Move other) const
{
    return origin == other.origin && destination == other.destination &&
           promotion == other.promotion && specialMoveFlag == other.specialMoveFlag;
}

SpecialMoveCompare::SpecialMoveCompare(Move move) : m_move(move)
{
}

bool SpecialMoveCompare::operator()(Move other)
{
    if (m_move.specialMoveFlag == 1 || other.specialMoveFlag == 1)
        return m_move.origin == other.origin && m_move.destination == other.destination &&
               m_move.promotion == other.promotion &&
               m_move.specialMoveFlag == other.specialMoveFlag;
    return m_move.origin == other.origin && m_move.destination == other.destination;
}

} // namespace chessAi