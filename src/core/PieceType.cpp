#include "PieceType.h"

namespace chessAi
{

PieceType::PieceType(PieceColor color, PieceFigure figure) : m_color(color), m_figure(figure)
{
}

PieceColor PieceType::getPieceColor() const
{
    return m_color;
}

PieceFigure PieceType::getPieceFigure() const
{
    return m_figure;
}

unsigned int PieceType::getPieceIndex() const
{
    return static_cast<int>(m_color) * (static_cast<int>(PieceFigure::_numberOfEnums) - 1) +
           static_cast<int>(m_figure) - 1;
}

bool PieceType::operator<(const PieceType& other) const
{
    return (static_cast<int>(m_color) * static_cast<int>(PieceFigure::_numberOfEnums)) +
               (static_cast<int>(m_figure) + 1) <
           (static_cast<int>(other.m_color) * static_cast<int>(PieceFigure::_numberOfEnums)) +
               (static_cast<int>(other.m_figure) + 1);
}

bool PieceType::operator==(const PieceType& other) const
{
    return m_color == other.m_color && m_figure == other.m_figure;
}

PieceColor PieceType::getOppositeColor(PieceColor color)
{
    return (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

} // namespace chessAi