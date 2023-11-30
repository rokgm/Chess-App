#include "PiecesSprites.h"
#include "logic/Pieces.h"

#include "spdlog/spdlog.h"

namespace chessAi
{

PiecesSprites::PiecesSprites()
{
    std::unordered_map<Pieces::Piece, std::string> pieceToFileName = {
        {Pieces::blackBishop, "black-bishop.png"}, {Pieces::blackKing, "black-king.png"},
        {Pieces::blackKnight, "black-knight.png"}, {Pieces::blackPawn, "black-pawn.png"},
        {Pieces::blackQueen, "black-queen.png"},   {Pieces::blackRook, "black-rook.png"},
        {Pieces::whiteBishop, "white-bishop.png"}, {Pieces::whiteKing, "white-king.png"},
        {Pieces::whiteKnight, "white-knight.png"}, {Pieces::whitePawn, "white-pawn.png"},
        {Pieces::whiteQueen, "white-queen.png"},   {Pieces::whiteRook, "white-rook.png"},
    };

    // for (auto piece : Pieces::allPieces) {
    //     // TODO: Sprite gets pointer to texture, implement so this doesn't go out of scope.
    //     sf::Texture texture;
    //     if (!texture.loadFromFile("pieces_images/white-pawn.png")) {
    //         SPDLOG_ERROR("File couldn't be opened.");
    //     }
    //     texture.setSmooth(true);
    //     sf::Sprite spri(texture);
    // }
}

} // namespace chessAi