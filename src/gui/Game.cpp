#include "Game.h"

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
            CHESS_LOG_ERROR("File {} couldn't be opened.", fileName);
        }
        texture.setSmooth(true);
        CHESS_LOG_TRACE("Texture loaded from file {}.", fileName);
        if (!textures.insert({piece, std::move(texture)}).second) {
            CHESS_LOG_ERROR("Texture {} couldn't be inserted to piece textures map.", fileName);
        }
    }

    return textures;
}

} // namespace

Game::Game(unsigned int windowWidth, unsigned int windowHeight)
    : m_windowSize(windowWidth, windowHeight),
      m_window(sf::RenderWindow(sf::VideoMode(m_windowSize.x, m_windowSize.y), "Chess Game")),
      m_board(Board(m_windowSize.getSmallestAxis() * 4 / 5)), m_boardPosition(BoardPosition()),
      m_pieceTextures(getPieceTextures())
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

void Game::movePiece(const sf::Event& event) 
{
    // Both x (starting left) and y (starting top) are part of the interval [1, 8], intead of [0, 7] as we are used to
    int position_x = event.mouseButton.x * 8 / static_cast<int>(m_board.getBoardSprite().getGlobalBounds().width); 
    int position_y = event.mouseButton.y * 8 / static_cast<int>(m_board.getBoardSprite().getGlobalBounds().height);
    int position_on_board = (position_x - 1) + (position_y - 1) * 8;

    std::unordered_map<BoardPosition::Piece, int> selectedPiece = m_boardPosition.getSelectedPiece();
    

    if (selectedPiece.count(BoardPosition::Piece::empty) == 1 || selectedPiece.empty())
    {
        if (m_boardPosition.getBoardPosition()[position_on_board] != BoardPosition::Piece::empty) {
            BoardPosition::Piece pieceType = m_boardPosition.getBoardPosition()[position_on_board];
            m_boardPosition.setSelectedPiece(pieceType, position_on_board);
        }
    } else {
        for (const auto& keyAndValue : selectedPiece) {
            m_boardPosition.getBoardPosition()[keyAndValue.second] = BoardPosition::Piece::empty;
            m_boardPosition.getBoardPosition()[position_on_board] = keyAndValue.first;
        }
        m_boardPosition.setSelectedPiece(BoardPosition::Piece::empty, -1);
    }



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
        case sf::Event::MouseButtonPressed:
            movePiece(event);
            break;
        default:
            break;
        }
    }
}

void Game::drawPieceType(BoardPosition::Piece type, int position,
                         const sf::FloatRect& boardGlobalBounds, float boardFieldSize)
{
    if (type == BoardPosition::Piece::empty) {
        return;
    }
    sf::Sprite pieceSprite(m_pieceTextures[type]);
    pieceSprite.scale(boardFieldSize / pieceSprite.getLocalBounds().width,
                      boardFieldSize / pieceSprite.getLocalBounds().height);
    pieceSprite.setPosition(
        boardGlobalBounds.left + boardFieldSize * static_cast<float>(position % 8),
        boardGlobalBounds.top + boardFieldSize * static_cast<float>(position / 8));
    m_window.draw(pieceSprite);
}

void Game::drawPosition()
{
    auto boardGlobalBounds = m_board.getBoardSprite().getGlobalBounds();
    auto boardFieldSize = boardGlobalBounds.width / 8;

    int position = 0;
    for (auto pieceType : m_boardPosition.getBoardPosition()) {
        drawPieceType(pieceType, position, boardGlobalBounds, boardFieldSize);
        ++position;
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