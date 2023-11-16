#pragma once

#include <SFML/Graphics.hpp>

#include <array>
namespace chessAi
{

class Board
{
public:
    Board(float fieldSize);

    const std::array<sf::RectangleShape, 64>& getBoard() const;

private:
    std::array<sf::RectangleShape, 64> m_board;
};

} // namespace chessAi