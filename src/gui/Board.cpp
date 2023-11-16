#include "Board.h"

namespace chessAi
{

Board::Board(float fieldSize)
{
    uint8_t fieldIndex = 0;

    for (auto& field : m_board) {
        uint8_t rowIndex = fieldIndex % 8;
        uint8_t columnIndex = fieldIndex / 8;

        sf::RectangleShape newField(sf::Vector2f(fieldSize, fieldSize));
        newField.setPosition(rowIndex * fieldSize, columnIndex * fieldSize);

        switch ((rowIndex + columnIndex) % 2) {
        case 0:
            // Set light brown color.
            newField.setFillColor(sf::Color::Color(254, 231, 193, 255));
            break;
        case 1:
            // Set dark brown color.
            newField.setFillColor(sf::Color::Color(150, 77, 34, 255));
            break;
        default:
            break;
        }

        field = newField;
        ++fieldIndex;
    }
}

const std::array<sf::RectangleShape, 64>& Board::getBoard() const
{
    return m_board;
}

} // namespace chessAi