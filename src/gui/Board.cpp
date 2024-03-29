#include "Board.h"

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

        if ((rowIndex + columnIndex) % 2 == 0) {
            // Set light brown color.
            newField.setFillColor(sf::Color(254, 231, 193, 255));
        }
        else {
            // Set dark brown color.
            newField.setFillColor(sf::Color(150, 77, 34, 255));
        }

        board.draw(newField);
    }
}

} // namespace

Board::Board(unsigned int boardSize) : m_boardSize(static_cast<float>(boardSize))
{
    if (!m_boardTexture.create(boardSize, boardSize)) {
        throw std::runtime_error("Board texture could not be created.");
    }
    m_boardTexture.clear();
    generateBoardFields(m_boardTexture, static_cast<float>(boardSize) / 8);
    m_boardTexture.display();
    m_boardSprite.setTexture(m_boardTexture.getTexture());
    m_boardSprite.setOrigin(static_cast<float>(m_boardSize) / 2,
                            static_cast<float>(m_boardSize) / 2);
}

const sf::Sprite& Board::getBoardSprite() const
{
    return m_boardSprite;
}

void Board::resize(float newBoardSize)
{
    m_boardSprite.setScale(newBoardSize / m_boardSprite.getLocalBounds().width,
                           newBoardSize / m_boardSprite.getLocalBounds().height);
    m_boardSize = newBoardSize;
}

void Board::setCenterPosition(const sf::Vector2u& windowSize)
{
    m_boardSprite.setPosition(0.5f * static_cast<sf::Vector2f>(windowSize));
}

} // namespace chessAi