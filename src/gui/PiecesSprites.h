#pragma once

#include "logic/Pieces.h"

#include <SFML/Graphics.hpp>

#include <unordered_map>

namespace chessAi
{

class PiecesSprites
{
public:
    PiecesSprites();

private:
    std::unordered_map<Pieces::Piece, sf::Sprite> m_sprites;
};

} // namespace chessAi