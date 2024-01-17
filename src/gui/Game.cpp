#include "Game.h"
#include "core/EndOfGameChecker.h"
#include "core/MoveGenerator.h"

#include <thread>

namespace chessAi
{

namespace
{

std::map<PieceType, sf::Texture> getPieceTextures()
{
    std::map<PieceType, std::string> pieceToFileName = {
        {PieceType(PieceColor::Black, PieceFigure::Bishop), "black-bishop.png"},
        {PieceType(PieceColor::Black, PieceFigure::King), "black-king.png"},
        {PieceType(PieceColor::Black, PieceFigure::Knight), "black-knight.png"},
        {PieceType(PieceColor::Black, PieceFigure::Pawn), "black-pawn.png"},
        {PieceType(PieceColor::Black, PieceFigure::Queen), "black-queen.png"},
        {PieceType(PieceColor::Black, PieceFigure::Rook), "black-rook.png"},
        {PieceType(PieceColor::White, PieceFigure::Bishop), "white-bishop.png"},
        {PieceType(PieceColor::White, PieceFigure::King), "white-king.png"},
        {PieceType(PieceColor::White, PieceFigure::Knight), "white-knight.png"},
        {PieceType(PieceColor::White, PieceFigure::Pawn), "white-pawn.png"},
        {PieceType(PieceColor::White, PieceFigure::Queen), "white-queen.png"},
        {PieceType(PieceColor::White, PieceFigure::Rook), "white-rook.png"},
    };

    std::map<PieceType, sf::Texture> textures;

    for (const auto& [piece, fileName] : pieceToFileName) {
        sf::Texture texture;
        if (!texture.loadFromFile("pieces_images/" + fileName)) {
            throw std::runtime_error(
                "Critical error loading piece textures, file couldn't be opened.");
        }
        texture.setSmooth(true);
        CHESS_LOG_TRACE("Texture loaded from file {}.", fileName);
        if (!textures.insert({piece, std::move(texture)}).second) {
            CHESS_LOG_ERROR("Texture {} couldn't be inserted to piece textures map.", fileName);
        }
    }

    return textures;
}

unsigned int getSmallestNumber(unsigned int x, unsigned int y)
{
    return (x < y) ? x : y;
}

} // namespace

Game::Game(sf::RenderWindow* window, PieceColor color, bool againstComputer,
           const std::string& fenString)
    : m_window(window),
      m_board(getSmallestNumber(m_window->getSize().x, m_window->getSize().y) * 4 / 5),
      m_pieceTextures(getPieceTextures()), m_selectedPiecePosition(std::nullopt),
      m_selectedPawnPromotion(std::nullopt), m_promotionDrawn(std::nullopt), m_playerColor(color),
      m_boardState(fenString), m_font(), m_instructions(), m_endOfGameText(),
      m_engine(std::chrono::milliseconds(3000)), m_againstComputer(againstComputer),
      m_engineThread(), m_engineIsRunning(false), m_runGame(true)
{
    m_board.setCenterPosition(m_window->getSize());

    if (!m_font.loadFromFile("fonts/arial_narrow_7.ttf")) {
        throw std::runtime_error("Game font couldn't be loaded.");
    }

    m_instructions.setFont(m_font);
    m_instructions.setString(
        "Use left arrow to go to previous positions.\nUse escape to go to menu.");
    m_instructions.setFillColor(sf::Color::White);

    m_endOfGameText.setFont(m_font);
    m_endOfGameText.setFillColor(sf::Color::White);
    m_endOfGameText.setOutlineColor(sf::Color::Black);
    m_endOfGameText.setOutlineThickness(10.f);
}

void Game::handleWindowResize(const sf::Event& event)
{
    m_window->setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                             static_cast<float>(event.size.height))));
    m_board.resize(getSmallestNumber(event.size.width, event.size.height) * 4 / 5);
    m_board.setCenterPosition(sf::Vector2u(event.size.width, event.size.height));
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

void Game::transformPositionDependingOnFirstMove(unsigned int& position)
{
    position = (m_playerColor == PieceColor::White) ? position : 63 - position;
}

void Game::handleMousePressed(const sf::Event& event)
{
    if (!determineMousePressedOnBoard(event.mouseButton.x, event.mouseButton.y)) {
        CHESS_LOG_TRACE("Mouse not pressed on board.");
        m_selectedPiecePosition = std::nullopt;
        return;
    }
    auto [positionX, positionY] =
        calculatePositionOnBoard(event.mouseButton.x, event.mouseButton.y);
    CHESS_LOG_TRACE("positionX: {}, positionY: {}", positionX, positionY);

    if (!m_promotionDrawn.has_value()) {
        if (m_selectedPiecePosition.has_value()) {
            movePieceOrSetPromotionMenu(positionX, positionY);
        }
        else {
            setSelectedPiece(positionX, positionY);
        }
    }
    else {
        setSelectedPawnPromotion(positionX, positionY);
        if (m_promotionDrawn.has_value())
            promotePawn(m_promotionDrawn->first);
    }
}

void Game::setSelectedPawnPromotion(unsigned int positionX, unsigned int positionY)
{
    if (!m_promotionDrawn.has_value()) {
        CHESS_LOG_ERROR("Promotion doesn't have a value.");
        return;
    }
    if (m_promotionDrawn->first > 63) {
        CHESS_LOG_ERROR("Promotion menu not drawn");
    }

    int positionMultiplicator;
    unsigned int position = m_promotionDrawn->first;
    unsigned int positionOnBoard = positionX + positionY * 8;
    transformPositionDependingOnFirstMove(positionOnBoard);

    if (position / 8 == 0)
        positionMultiplicator = 1;
    else
        positionMultiplicator = -1;

    if (positionOnBoard == position) {
        m_selectedPawnPromotion = 0;
    }
    else if (positionOnBoard == position + 8 * positionMultiplicator) {
        m_selectedPawnPromotion = 1;
    }
    else if (positionOnBoard == position + 16 * positionMultiplicator) {
        m_selectedPawnPromotion = 2;
    }
    else if (positionOnBoard == position + 24 * positionMultiplicator) {
        m_selectedPawnPromotion = 3;
    }

    if (!m_selectedPawnPromotion.has_value()) {
        CHESS_LOG_TRACE("Promotion was not successful");
        m_promotionDrawn = std::nullopt;
    }
}

void Game::promotePawn(unsigned int positionOnBoard)
{
    if (!m_selectedPawnPromotion.has_value()) {
        return;
    }
    if (!m_selectedPiecePosition.has_value()) {
        CHESS_LOG_TRACE(
            "There is no selected piece. You probably pressed the mouse outside the board!");
    }
    auto endOfGame = m_boardState.updateBoardState(Move(
        static_cast<uint16_t>(*m_selectedPiecePosition), static_cast<uint16_t>(positionOnBoard),
        static_cast<uint16_t>(*m_selectedPawnPromotion), 1));
    m_selectedPiecePosition = std::nullopt;
    m_selectedPawnPromotion = std::nullopt;
    m_promotionDrawn = std::nullopt;

    if (endOfGame == EndOfGameType::Checkmate)
        m_endOfGameText.setString("CHECKMATE");
    else if (endOfGame == EndOfGameType::Stalemate)
        m_endOfGameText.setString("STALEMATE");
}

void Game::setSelectedPiece(unsigned int positionX, unsigned int positionY)
{
    unsigned int positionOnBoard = positionX + positionY * 8;
    transformPositionDependingOnFirstMove(positionOnBoard);
    auto type = m_boardState.getPiece(static_cast<uint64_t>(positionOnBoard));
    if (type.getPieceFigure() != PieceFigure::Empty) {
        m_selectedPiecePosition = positionOnBoard;
    }
}

void Game::movePieceOrSetPromotionMenu(unsigned int positionX, unsigned int positionY)
{
    unsigned int newPosition = positionX + positionY * 8;
    transformPositionDependingOnFirstMove(newPosition);
    positionX = newPosition % 8;
    positionY = newPosition / 8;
    if (!m_selectedPiecePosition.has_value()) {
        CHESS_LOG_ERROR("Moving piece with no current (old) position.");
        return;
    }

    auto selectedFigure = m_boardState.getPiece(*m_selectedPiecePosition);
    if (selectedFigure.getPieceFigure() == PieceFigure::Pawn &&
        ((positionY == 0 && selectedFigure.getPieceColor() == PieceColor::White) ||
         (positionY == 7 && selectedFigure.getPieceColor() == PieceColor::Black))) {
        if (selectedFigure.getPieceColor() == m_boardState.getBitBoards().currentMoveColor) {
            auto newPositionFigure = m_boardState.getPiece(newPosition);
            auto selectedPiecePosition = *m_selectedPiecePosition;
            if (((newPositionFigure.getPieceColor() != selectedFigure.getPieceColor() &&
                  (positionX == selectedPiecePosition % 8 + 1 ||
                   positionX == selectedPiecePosition % 8 - 1))) ||
                (newPositionFigure.getPieceFigure() == PieceFigure::Empty &&
                 positionX == selectedPiecePosition % 8)) {
                m_promotionDrawn = {newPosition, selectedFigure.getPieceColor()};
            }
        }
        else {
            m_selectedPiecePosition = std::nullopt;
        }
    }
    else {
        auto endOfGame =
            m_boardState.updateBoardState(Move(static_cast<uint16_t>(*m_selectedPiecePosition),
                                               static_cast<uint16_t>(newPosition), 0, 0));
        m_selectedPiecePosition = std::nullopt;

        if (endOfGame == EndOfGameType::Checkmate)
            m_endOfGameText.setString("CHECKMATE");
        else if (endOfGame == EndOfGameType::Stalemate)
            m_endOfGameText.setString("STALEMATE");
    }
}

void Game::handleEvents()
{
    sf::Event event;
    while (m_window->pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_window->close();
            break;
        case sf::Event::Resized:
            handleWindowResize(event);
            break;
        case sf::Event::MouseButtonPressed:
            if (m_engineIsRunning)
                break;
            if (m_endOfGameText.getString().isEmpty())
                handleMousePressed(event);
            break;
        case sf::Event::KeyPressed:
            if (m_engineIsRunning)
                break;
            if (event.key.scancode == sf::Keyboard::Scan::Left) {
                CHESS_LOG_TRACE("Left arrow pressed.");
                m_boardState.goToPreviousBoardState();
                // If against computer, previous computer move must also be skipped.
                if (m_againstComputer)
                    m_boardState.goToPreviousBoardState();
                m_endOfGameText.setString("");
                m_promotionDrawn = std::nullopt;
                m_selectedPiecePosition = std::nullopt;
            }
            else if (event.key.scancode == sf::Keyboard::Scan::Escape) {
                m_runGame = false;
            }
            break;
        default:
            break;
        }
    }
}

sf::RectangleShape Game::getBoardFieldHighlight(unsigned int position, const sf::Color& color)
{
    transformPositionDependingOnFirstMove(position);
    unsigned int boardRow = position / 8;
    unsigned int boardColumn = position % 8;
    auto boardFieldSize = m_board.getBoardSprite().getGlobalBounds().width / 8;
    sf::RectangleShape highlight(sf::Vector2f(boardFieldSize, boardFieldSize));
    // Set transparent orange color.
    highlight.setFillColor(color);
    highlight.setPosition(sf::Vector2f(m_board.getBoardSprite().getGlobalBounds().left,
                                       m_board.getBoardSprite().getGlobalBounds().top) +
                          boardFieldSize * sf::Vector2f(static_cast<float>(boardColumn),
                                                        static_cast<float>(boardRow)));
    return highlight;
}

void Game::drawPromotion(PieceColor color, unsigned int positionX, unsigned int positionY)
{
    // Making the background transparent
    int sizeX = m_window->getSize().x;
    int sizeY = m_window->getSize().y;
    sf::Texture capturedWindow;
    capturedWindow.create(sizeX, sizeY);
    capturedWindow.update(*m_window);
    sf::Sprite transparentWindowSprite(capturedWindow);
    transparentWindowSprite.setColor(sf::Color(0, 0, 0, 130));
    m_window->draw(transparentWindowSprite);

    sf::Sprite knight(m_pieceTextures.at(PieceType(color, PieceFigure::Knight)));
    sf::Sprite bishop(m_pieceTextures.at(PieceType(color, PieceFigure::Bishop)));
    sf::Sprite rook(m_pieceTextures.at(PieceType(color, PieceFigure::Rook)));
    sf::Sprite queen(m_pieceTextures.at(PieceType(color, PieceFigure::Queen)));

    int colorMultiplicator;
    if (color == m_playerColor)
        colorMultiplicator = 1;
    else
        colorMultiplicator = -1;

    auto boardGlobalBounds = m_board.getBoardSprite().getGlobalBounds();
    auto boardFieldSize = boardGlobalBounds.width / 8;
    int position = 0;
    for (auto figure : {knight, bishop, rook, queen}) {
        figure.scale(boardFieldSize / figure.getLocalBounds().width,
                     boardFieldSize / figure.getLocalBounds().height);
        figure.setPosition(boardGlobalBounds.left + boardFieldSize * static_cast<float>(positionX),
                           boardGlobalBounds.top +
                               static_cast<float>(positionY + position * colorMultiplicator) *
                                   boardFieldSize);
        m_window->draw(figure);
        position++;
    }
}

void Game::drawPieceType(const PieceType& type, unsigned int position,
                         const sf::FloatRect& boardGlobalBounds, float boardFieldSize)
{
    if (type.getPieceFigure() == PieceFigure::Empty) {
        return;
    }
    transformPositionDependingOnFirstMove(position);
    sf::Sprite pieceSprite(m_pieceTextures.at(type));
    pieceSprite.scale(boardFieldSize / pieceSprite.getLocalBounds().width,
                      boardFieldSize / pieceSprite.getLocalBounds().height);
    pieceSprite.setPosition(
        boardGlobalBounds.left + boardFieldSize * static_cast<float>(position % 8),
        boardGlobalBounds.top + boardFieldSize * static_cast<float>(position / 8));
    m_window->draw(pieceSprite);
}

void Game::drawPieceOnPositions(const PieceType& type, const std::vector<unsigned int>& positions,
                                const sf::FloatRect& boardGlobalBounds)
{
    auto boardFieldSize = boardGlobalBounds.width / 8;
    for (auto position : positions) {
        Game::drawPieceType(type, position, boardGlobalBounds, boardFieldSize);
    }
}

void Game::drawPosition()
{
    auto boardGlobalBounds = m_board.getBoardSprite().getGlobalBounds();
    for (const auto& [pieceType, bitBoard] :
         m_boardState.getBitBoards().getTypeToPieceBitBoards()) {
        drawPieceOnPositions(pieceType, PieceBitBoards::getSetBitPositions(*bitBoard),
                             boardGlobalBounds);
    }
}

void Game::highlightLegalMoves(unsigned int position)
{
    auto piece = m_boardState.getPiece(position);
    switch (piece.getPieceColor()) {
    case PieceColor::White:
        switch (piece.getPieceFigure()) {
        case PieceFigure::Pawn:
            drawMoveDestinationHighlights<PieceColor::White, PieceFigure::Pawn>(position);
            break;
        case PieceFigure::Bishop:
            drawMoveDestinationHighlights<PieceColor::White, PieceFigure::Bishop>(position);
            break;
        case PieceFigure::Rook:
            drawMoveDestinationHighlights<PieceColor::White, PieceFigure::Rook>(position);
            break;
        case PieceFigure::Knight:
            drawMoveDestinationHighlights<PieceColor::White, PieceFigure::Knight>(position);
            break;
        case PieceFigure::King:
            drawMoveDestinationHighlights<PieceColor::White, PieceFigure::King>(position);
            break;
        case PieceFigure::Queen:
            drawMoveDestinationHighlights<PieceColor::White, PieceFigure::Queen>(position);
            break;
        default:
            CHESS_LOG_ERROR("Unhandled piece figure.");
            break;
        }
        break;
    case PieceColor::Black:
        switch (piece.getPieceFigure()) {
        case PieceFigure::Pawn:
            drawMoveDestinationHighlights<PieceColor::Black, PieceFigure::Pawn>(position);
            break;
        case PieceFigure::Bishop:
            drawMoveDestinationHighlights<PieceColor::Black, PieceFigure::Bishop>(position);
            break;
        case PieceFigure::Rook:
            drawMoveDestinationHighlights<PieceColor::Black, PieceFigure::Rook>(position);
            break;
        case PieceFigure::Knight:
            drawMoveDestinationHighlights<PieceColor::Black, PieceFigure::Knight>(position);

            break;
        case PieceFigure::King:
            drawMoveDestinationHighlights<PieceColor::Black, PieceFigure::King>(position);
            break;
        case PieceFigure::Queen:
            drawMoveDestinationHighlights<PieceColor::Black, PieceFigure::Queen>(position);
            break;
        default:
            CHESS_LOG_ERROR("Unhandled piece figure.");
            break;
        }
        break;
    default:
        CHESS_LOG_ERROR("Unhandled piece color.");
        break;
    }
}

void Game::highlightKingInCheck()
{
    if (MoveGenerator<PieceColor::White>::isKingInCheck(m_boardState.getBitBoards())) {
        for (auto position :
             PieceBitBoards::getSetBitPositions(m_boardState.getBitBoards().whiteKing)) {
            m_window->draw(getBoardFieldHighlight(position, sf::Color(220, 20, 60, 125)));
        }
    }
    if (MoveGenerator<PieceColor::Black>::isKingInCheck(m_boardState.getBitBoards())) {
        for (auto position :
             PieceBitBoards::getSetBitPositions(m_boardState.getBitBoards().blackKing)) {
            m_window->draw(getBoardFieldHighlight(position, sf::Color(220, 20, 60, 125)));
        }
    }
}

void Game::drawInstructions()
{
    auto boardBounds = m_board.getBoardSprite().getGlobalBounds();
    m_instructions.setPosition(boardBounds.left, boardBounds.top + boardBounds.height);
    m_instructions.setCharacterSize(static_cast<unsigned int>(
        0.03f *
        static_cast<float>(getSmallestNumber(m_window->getSize().x, m_window->getSize().y))));
    m_window->draw(m_instructions);
}

void Game::drawGameOver()
{
    if (!m_endOfGameText.getString().isEmpty()) {
        m_endOfGameText.setOrigin(0.5f * m_endOfGameText.getLocalBounds().getSize());
        m_endOfGameText.setPosition(0.5f * static_cast<sf::Vector2f>(m_window->getSize()));
        m_endOfGameText.setCharacterSize(static_cast<unsigned int>(
            0.2f *
            static_cast<float>(getSmallestNumber(m_window->getSize().x, m_window->getSize().y))));
        m_window->draw(m_endOfGameText);
    }
}

void Game::drawPromotion()
{
    if (m_promotionDrawn.has_value()) {
        unsigned int position = m_promotionDrawn->first;
        transformPositionDependingOnFirstMove(position);
        unsigned int positionX = position % 8;
        unsigned int positionY = position / 8;
        drawPromotion(m_promotionDrawn->second, positionX, positionY);
    }
}

void Game::displayGameSprites()
{
    // Set  grey background color.
    m_window->clear(sf::Color(128, 128, 128, 255));

    const auto& boardSprite = m_board.getBoardSprite();
    m_window->draw(boardSprite);
    if (m_selectedPiecePosition.has_value()) {
        m_window->draw(getBoardFieldHighlight(*m_selectedPiecePosition));
        highlightLegalMoves(*m_selectedPiecePosition);
    }
    auto move = m_boardState.getLastMove();
    if (move.has_value()) {
        m_window->draw(getBoardFieldHighlight(move->origin));
        m_window->draw(getBoardFieldHighlight(move->destination));
    }
    highlightKingInCheck();
    drawPosition();
    drawInstructions();
    drawPromotion();
    drawGameOver();

    m_window->display();
}

void Game::runEngine()
{
    m_engineIsRunning = true;

    auto [move, depth] = m_engine.findBestMove(m_boardState.getBitBoards());
    CHESS_LOG_INFO("Depth to which the engine searched is {}", depth);
    if (move.has_value()) {
        auto endOfGame = m_boardState.updateBoardState(*move);

        if (endOfGame == EndOfGameType::Checkmate)
            m_endOfGameText.setString("CHECKMATE");
        else if (endOfGame == EndOfGameType::Stalemate)
            m_endOfGameText.setString("STALEMATE");
    }
    else
        CHESS_LOG_ERROR("Engine didn't return move in position.");

    m_engineIsRunning = false;
}

void Game::runGame()
{
    while (m_window->isOpen() && m_runGame) {
        handleEvents();
        displayGameSprites();
        if (m_againstComputer && !m_engineIsRunning) {
            if (m_engineThread.joinable())
                m_engineThread.join();
            else {
                if (m_playerColor != m_boardState.getBitBoards().currentMoveColor) {
                    m_engineThread = std::thread(&Game::runEngine, this);
                }
            }
        }
    }
}

} // namespace chessAi