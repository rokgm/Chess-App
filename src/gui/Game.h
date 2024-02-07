#pragma once

#include "Board.h"
#include "core/BoardState.h"
#include "core/Engine.h"

#include <SFML/Graphics.hpp>

#include <optional>
#include <unordered_map>

namespace chessAi
{

class Game
{
public:
    /**
     * Construct a new Game object, that will run the gui window and the game logic.
     */
    Game(sf::RenderWindow* window, PieceColor color, bool againstComputer,
         const std::chrono::milliseconds& timeLimit, const std::string& fenString);

    void runGame();

private:
    void handleEvents();
    void displayGameSprites();
    void runEngine();
    void handleWindowResize(const sf::Event& event);
    void handleMousePressed(const sf::Event& event);
    bool determineMousePressedOnBoard(int x, int y);
    void movePieceOrSetPromotionMenu(unsigned int positionX, unsigned int positionY);
    void setSelectedPiece(unsigned int positionX, unsigned int positionY);
    void setSelectedPawnPromotion(unsigned int positionX, unsigned int positionY);
    void promotePawn(unsigned int positionOnBoard);
    std::pair<unsigned int, unsigned int> calculatePositionOnBoard(unsigned int x, unsigned int y);
    void drawPromotion(PieceColor color, unsigned int positionX, unsigned int positionY);
    void drawPosition();
    void drawPieceOnPositions(const PieceType& type, const std::vector<uint16_t>& positions,
                              const sf::FloatRect& boardGlobalBounds);
    void drawPieceType(const PieceType& type, unsigned int position,
                       const sf::FloatRect& boardGlobalBounds, float boardFieldSize);
    sf::RectangleShape getBoardFieldHighlight(unsigned int position,
                                              const sf::Color& color = sf::Color(255, 178, 102,
                                                                                 150));
    void highlightLegalMoves(unsigned int position);
    void highlightKingInCheck();
    template <PieceColor TColor>
    void drawMoveDestinationHighlights(unsigned int position, PieceFigure figure);
    void drawInstructions();
    void drawGameOver();
    void drawPromotion();
    bool findPieceCapturedNotation(const PieceBitBoards& bitBoards, uint16_t destination);
    char findPieceNotation(const PieceBitBoards& bitBoards, uint16_t destination);
    std::string getWholeMoveString(Move move, bool figureWasCaptured, char selectedPieceNotation);
    void findAlgebraicNotation();
    void drawGameHistory();
    void drawGameHistoryBackground(sf::FloatRect& boardBounds);
    void drawGameHistoryMoves();
    void setGameHistoryView(sf::RectangleShape& gameHistoryBackground);
    void transformPositionDependingOnFirstMove(unsigned int& position);

private:
    sf::RenderWindow* m_window;
    Board m_board;
    std::map<PieceType, sf::Texture> m_pieceTextures;
    std::optional<unsigned int> m_selectedPiecePosition;
    std::optional<unsigned int> m_selectedPawnPromotion;
    std::optional<std::pair<unsigned int, PieceColor>> m_promotionDrawn;
    PieceColor m_playerColor;
    BoardState m_boardState;
    sf::Font m_font;
    sf::Text m_instructions;
    sf::Text m_endOfGameText;
    sf::Text m_showGameHistoryText;
    sf::Text m_whiteMovesText;
    sf::Text m_blackMovesText;
    sf::Text m_gameHistoryWhiteText;
    sf::Text m_gameHistoryBlackText;
    sf::RectangleShape m_gameHistoryBackground;
    sf::View m_gameHistoryView;
    Engine m_engine;
    bool m_againstComputer;
    std::thread m_engineThread;
    std::atomic<bool> m_engineIsRunning;
    bool m_runGame;
    bool m_showGameHistory;
    int m_mouseScrolled;
    std::array<char, 6> m_figureNotation;
    std::string m_whiteMovesString;
    std::string m_blackMovesString;
};

template <PieceColor TColor>
void Game::drawMoveDestinationHighlights(unsigned int position, PieceFigure figure)
{
    auto moves = MoveGenerator<TColor>::template generateLegalMoves<MoveType::Normal>(
        m_boardState.getBitBoards(), figure, static_cast<uint16_t>(position));

    for (const auto& move : moves) {
        m_window->draw(getBoardFieldHighlight(move.destination));
    }
}

} // namespace chessAi