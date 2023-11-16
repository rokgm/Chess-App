#include "Board.h"

#include "spdlog/spdlog.h"

namespace chessAi
{

namespace
{

void generateBoardFields(sf::RenderTexture& board, float fieldSize)
{
    for (uint8_t fieldIndex = 0; fieldIndex < 64; ++fieldIndex) {
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
        board.draw(newField);
    }
}

} // namespace

Board::Board(unsigned int boardSize) : m_boardSize(boardSize)
{
    if (!m_boardTexture.create(m_boardSize, m_boardSize)) {
        SPDLOG_ERROR("Board texture could not be created.");
    }
    m_boardTexture.clear();
    generateBoardFields(m_boardTexture, static_cast<float>(boardSize) / 8);
    m_boardTexture.display();
}

sf::Sprite Board::getBoardSprite() const
{
    sf::Sprite board(m_boardTexture.getTexture());
    board.setOrigin(static_cast<float>(m_boardSize) / 2, static_cast<float>(m_boardSize) / 2);
    return board;
}

void Board::boardResize(unsigned int newSize)
{
    m_boardSize = newSize;
    m_boardTexture.clear();
    generateBoardFields(m_boardTexture, static_cast<float>(newSize) / 8);
    m_boardTexture.display();
}

} // namespace chessAi