#include "Game.h"

#include "spdlog/spdlog.h"

namespace chessAi
{

namespace
{

std::unordered_map<BoardPosition::Piece, sf::Texture> getPieceTextures()
{
    std::unordered_map<BoardPosition::Piece, std::string> pieceToFileName = {
        {BoardPosition::Piece::blackBishop, "black-bishop.png"},
        {BoardPosition::Piece::blackKing, "black-king.png"},
        {BoardPosition::Piece::blackKnight, "black-knight.png"},
        {BoardPosition::Piece::blackPawn, "black-pawn.png"},
        {BoardPosition::Piece::blackQueen, "black-queen.png"},
        {BoardPosition::Piece::blackRook, "black-rook.png"},
        {BoardPosition::Piece::whiteBishop, "white-bishop.png"},
        {BoardPosition::Piece::whiteKing, "white-king.png"},
        {BoardPosition::Piece::whiteKnight, "white-knight.png"},
        {BoardPosition::Piece::whitePawn, "white-pawn.png"},
        {BoardPosition::Piece::whiteQueen, "white-queen.png"},
        {BoardPosition::Piece::whiteRook, "white-rook.png"},
    };

    std::unordered_map<BoardPosition::Piece, sf::Texture> textures;

    for (const auto& [piece, fileName] : pieceToFileName) {
        sf::Texture texture;
        if (!texture.loadFromFile("pieces_images/" + fileName)) {
            SPDLOG_ERROR("File {} couldn't be opened.", fileName);
        }
        texture.setSmooth(true);
        SPDLOG_TRACE("Texture loaded from file {}.", fileName);
        if (!textures.insert({piece, std::move(texture)}).second) {
            SPDLOG_ERROR("Texture {} couldn't be inserted to piece textures map.", fileName);
        }
    }

    return textures;
}

} // namespace

Game::Game(unsigned int windowWidth, unsigned int windowHeight)
    : m_windowSize(windowWidth, windowHeight),
      m_window(sf::RenderWindow(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Chess Game")),
      m_board(Board(m_windowSize.getSmallestAxis() * 4 / 5)), m_pieceTextures(getPieceTextures()),
      m_boardPosition(BoardPosition())
{
    m_window.setVerticalSyncEnabled(true);
    m_board.setCenterPosition(m_windowSize);
}

void Game::handleWindowResize(const sf::Event& event)
{
    m_window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                            static_cast<float>(event.size.height))));
    m_windowSize.x = event.size.width;
    m_windowSize.y = event.size.height;
    m_board.resize(m_windowSize.getSmallestAxis() * 4 / 5);
    m_board.setCenterPosition(m_windowSize);
}

void Game::handleEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_window.close();
            break;
        case sf::Event::Resized:
            handleWindowResize(event);
            break;
        default:
            break;
        }
    }
}

void Game::drawPieceType(const std::vector<int>& positions, sf::Sprite& pieceSprite,
                         const sf::FloatRect& boardGlobalBounds, const float& boardFieldSize)
{
    for (const auto& position : positions) {
        pieceSprite.setPosition(boardGlobalBounds.left + boardFieldSize * (position % 8),
                                boardGlobalBounds.top + boardFieldSize * (position / 8));
        m_window.draw(pieceSprite);
    }
}

void Game::drawPosition()
{
    auto boardGlobalBounds = m_board.getBoardSprite().getGlobalBounds();
    auto boardFieldSize = boardGlobalBounds.width / 8;

    for (const auto& [pieceType, bitPiecePositions] : m_boardPosition.getBoardPosition()) {
        auto integerPositions = BoardPosition::findSetBits(bitPiecePositions);
        sf::Sprite pieceSprite(m_pieceTextures[pieceType]);
        pieceSprite.scale(boardFieldSize / pieceSprite.getLocalBounds().width,
                          boardFieldSize / pieceSprite.getLocalBounds().height);
        drawPieceType(integerPositions, pieceSprite, boardGlobalBounds, boardFieldSize);
    }
}

void Game::displayGameSprites()
{
    // Set  grey background color.
    m_window.clear(sf::Color(128, 128, 128, 255));

    const auto& boardSprite = m_board.getBoardSprite();
    m_window.draw(boardSprite);
    drawPosition();
    m_window.display();
}

void Game::runGame()
{
    while (m_window.isOpen()) {
        handleEvents();
        displayGameSprites();
    }
}

} // namespace chessAi