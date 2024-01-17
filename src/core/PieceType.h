#pragma once

#include <array>

namespace chessAi
{

enum class PieceColor
{
    White,
    Black
};

enum class PieceFigure
{
    Empty = 0,
    Pawn,
    Bishop,
    Knight,
    Rook,
    King,
    Queen,
    _numberOfEnums = 7
};
class PieceType
{
public:
    PieceType(PieceColor color, PieceFigure figure);

    PieceColor getPieceColor() const;
    PieceFigure getPieceFigure() const;
    /**
     * Get piece index from 0-11, empty figure not included.
     */
    unsigned int getPieceIndex() const;

    bool operator<(const PieceType& other) const;
    bool operator==(const PieceType& other) const;

    template <PieceColor TColor>
    static constexpr PieceColor getOppositeColor();

    static PieceColor getOppositeColor(PieceColor color);

private:
    PieceColor m_color;
    PieceFigure m_figure;
};

template <PieceColor TColor>
constexpr PieceColor PieceType::getOppositeColor()
{
    return (TColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

} // namespace chessAi