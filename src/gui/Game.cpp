#include "Game.h"

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
        sf::Texture texture;
        if (!texture.loadFromFile("pieces_images/" + fileName)) {
            CHESS_LOG_ERROR("File {} couldn't be opened.", fileName);
            throw std::runtime_error("Critical error loading piece textures.");
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
      m_board(Board(m_windowSize.getSmallestAxis() * 4 / 5)), m_boardState(BoardState()),
      m_pieceTextures(getPieceTextures()),
      m_selectedPieceAndPosition({BoardState::Piece::empty, std::nullopt})
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

bool Game::determineMousePressedOnBoard(int x, int y)
{
    auto bounds = m_board.getBoardSprite().getGlobalBounds();
    return bounds.left < static_cast<float>(x) &&
           static_cast<float>(x) < (bounds.width + bounds.left) &&
           bounds.top < static_cast<float>(y) &&
           static_cast<float>(y) < (bounds.height + bounds.top);
}

std::pair<unsigned int, unsigned int> Game::calculatePositionOnBoard(unsigned int x, unsigned int y)
{
    // Both x (starting left) and y (starting top) are part of the interval [0, 7].
    auto boardBounds = m_board.getBoardSprite().getGlobalBounds();
    unsigned int positionX = static_cast<unsigned int>((static_cast<float>(x) - boardBounds.left) *
                                                       8 / boardBounds.width);
    unsigned int positionY = static_cast<unsigned int>((static_cast<float>(y) - boardBounds.top) *
                                                       8 / boardBounds.height);
    return {std::move(positionX), std::move(positionY)};
}

void Game::handleMousePressed(const sf::Event& event)
{
    if (!determineMousePressedOnBoard(event.mouseButton.x, event.mouseButton.y)) {
        CHESS_LOG_TRACE("Mouse not pressed on board.");
        m_selectedPieceAndPosition.second = std::nullopt;
        return;
    }
    auto [positionX, positionY] =
        calculatePositionOnBoard(event.mouseButton.x, event.mouseButton.y);
    CHESS_LOG_TRACE("positionX: {}, positionY: {}", positionX, positionY);

    if (m_selectedPieceAndPosition.second.has_value()) {
        movePiece(positionX, positionY);
    }
    else {
        setSelectedPiece(positionX, positionY);
    }
}

void Game::setSelectedPiece(unsigned int positionX, unsigned int positionY)
{
    unsigned int positionOnBoard = positionX + positionY * 8;
    if (m_boardState.getBoardState()[positionOnBoard] != BoardState::Piece::empty) {
        BoardState::Piece pieceType = m_boardState.getBoardState()[positionOnBoard];
        m_selectedPieceAndPosition = {pieceType, positionOnBoard};
    }
}

void Game::movePiece(unsigned int positionX, unsigned int positionY)
{

    int positionOnBoard = positionX + positionY * 8;
    if (!m_selectedPieceAndPosition.second.has_value()) {
        CHESS_LOG_ERROR("Moving piece with no current (old) position.");
        return;
    }
    m_boardState.updateBoardState(m_selectedPieceAndPosition.first,
                                  *m_selectedPieceAndPosition.second, positionOnBoard);
    m_selectedPieceAndPosition = {BoardState::Piece::empty, std::nullopt};
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
            handleMousePressed(event);
            break;
        default:
            break;
        }
    }
}

sf::RectangleShape Game::getBoardFieldHighlight(unsigned int boardRow, unsigned int boardColumn)
{
    auto boardFieldSize = m_board.getBoardSprite().getGlobalBounds().width / 8;
    sf::RectangleShape highlight(sf::Vector2f(boardFieldSize, boardFieldSize));
    // Set transparent orange color.
    highlight.setFillColor(sf::Color(255, 127, 80, 125));
    highlight.setPosition(sf::Vector2f(m_board.getBoardSprite().getGlobalBounds().left,
                                       m_board.getBoardSprite().getGlobalBounds().top) +
                          boardFieldSize * sf::Vector2f(static_cast<float>(boardColumn),
                                                        static_cast<float>(boardRow)));
    return highlight;
}

void Game::drawPieceType(BoardState::Piece type, unsigned int position,
                         const sf::FloatRect& boardGlobalBounds, float boardFieldSize)
{
    if (type == BoardState::Piece::empty) {
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

    if (m_selectedPieceAndPosition.second.has_value()) {
        auto position = *m_selectedPieceAndPosition.second;
        m_window.draw(getBoardFieldHighlight(position / 8, position % 8));
    }

    int position = 0;
    for (auto pieceType : m_boardState.getBoardState()) {
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