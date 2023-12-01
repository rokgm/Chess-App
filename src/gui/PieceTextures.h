#pragma once

#include "logic/Pieces.h"

#include <SFML/Graphics.hpp>

#include <unordered_map>

namespace chessAi
{

class PieceTextures
{
public:
    static std::unordered_map<Pieces::Piece, sf::Texture> getPieceTextures();
};

} // namespace chessAi