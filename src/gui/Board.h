#pragma once

#include <SFML/Graphics.hpp>

#include <array>
namespace chessAi
{

class Board
{
public:
    Board(float fieldSize);

    const std::array<std::array<sf::RectangleShape, 8>, 8>& getBoard() const;

private:
    std::array<std::array<sf::RectangleShape, 8>, 8> m_board;
};

} // namespace chessAi