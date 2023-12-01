#include "PieceTextures.h"
#include "logic/Pieces.h"

#include "spdlog/spdlog.h"

namespace chessAi
{

std::unordered_map<Pieces::Piece, sf::Texture> PieceTextures::getPieceTextures()
{
    std::unordered_map<Pieces::Piece, std::string> pieceToFileName = {
        {Pieces::Piece::blackBishop, "black-bishop.png"},
        {Pieces::Piece::blackKing, "black-king.png"},
        {Pieces::Piece::blackKnight, "black-knight.png"},
        {Pieces::Piece::blackPawn, "black-pawn.png"},
        {Pieces::Piece::blackQueen, "black-queen.png"},
        {Pieces::Piece::blackRook, "black-rook.png"},
        {Pieces::Piece::whiteBishop, "white-bishop.png"},
        {Pieces::Piece::whiteKing, "white-king.png"},
        {Pieces::Piece::whiteKnight, "white-knight.png"},
        {Pieces::Piece::whitePawn, "white-pawn.png"},
        {Pieces::Piece::whiteQueen, "white-queen.png"},
        {Pieces::Piece::whiteRook, "white-rook.png"},
    };

    std::unordered_map<Pieces::Piece, sf::Texture> textures;

    for (const auto& [piece, fileName] : pieceToFileName) {
        auto& texture = textures[piece];
        if (!texture.loadFromFile("pieces_images/" + fileName)) {
            SPDLOG_ERROR("File {} couldn't be opened.", fileName);
        }
        texture.setSmooth(true);
        SPDLOG_TRACE("Texture loaded from file {}.", fileName);
    }

    return textures;
}

} // namespace chessAi