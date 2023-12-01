#include "Game.h"

#include "spdlog/spdlog.h"

namespace chessAi
{

namespace
{

std::unordered_map<BoardState::Piece, sf::Texture> getPieceTextures()
{
    std::unordered_map<BoardState::Piece, std::string> pieceToFileName = {
        {BoardState::Piece::blackBishop, "black-bishop.png"},
        {BoardState::Piece::blackKing, "black-king.png"},
        {BoardState::Piece::blackKnight, "black-knight.png"},
        {BoardState::Piece::blackPawn, "black-pawn.png"},
        {BoardState::Piece::blackQueen, "black-queen.png"},
        {BoardState::Piece::blackRook, "black-rook.png"},
        {BoardState::Piece::whiteBishop, "white-bishop.png"},
        {BoardState::Piece::whiteKing, "white-king.png"},
        {BoardState::Piece::whiteKnight, "white-knight.png"},
        {BoardState::Piece::whitePawn, "white-pawn.png"},
        {BoardState::Piece::whiteQueen, "white-queen.png"},
        {BoardState::Piece::whiteRook, "white-rook.png"},
    };

    std::unordered_map<BoardState::Piece, sf::Texture> textures;

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

} // namespace

Game::Game(unsigned int windowWidth, unsigned int windowHeight)
    : m_windowSize(windowWidth, windowHeight),
      m_window(sf::RenderWindow(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Chess Game")),
      m_board(Board(m_windowSize.getSmallestAxis() * 4 / 5)), m_pieceTextures(getPieceTextures())
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

void Game::drawGameTextures()
{
    // Set  grey background color.
    m_window.clear(sf::Color(128, 128, 128, 255));

    const auto& boardSprite = m_board.getBoardSprite();
    m_window.draw(boardSprite);

    // TODO: Improve this test drawing and move to function that draws from board state.
    float x = 100.f;
    float y = 100.f;
    for (const auto& [piece, texture] : m_pieceTextures) {
        sf::Sprite sprite(texture);
        sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
        sprite.setPosition(x, y);
        m_window.draw(sprite);
        x += 50.f;
        y += 50.f;
    }

    m_window.display();
}

void Game::runGame()
{
    while (m_window.isOpen()) {
        handleEvents();
        drawGameTextures();
    }
}

} // namespace chessAi