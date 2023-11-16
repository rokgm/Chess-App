#include "Board.h"

namespace chessAi
{

Board::Board(float fieldSize)
{
    uint8_t rowIndex = 0;
    uint8_t columnIndex = 0;

    for (auto& row : m_board) {
        for (auto& field : row) {

            sf::RectangleShape newField(sf::Vector2f(fieldSize, fieldSize));
            newField.setPosition(columnIndex * fieldSize, rowIndex * fieldSize);

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
            ++columnIndex;
        }
        columnIndex = 0;
        ++rowIndex;
    }
}

const std::array<std::array<sf::RectangleShape, 8>, 8>& Board::getBoard() const
{
    return m_board;
}

} // namespace chessAi