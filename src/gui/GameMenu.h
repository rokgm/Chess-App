#pragma once

#include "core/PieceType.h"

#include <SFML/Graphics.hpp>

#include <optional>

namespace chessAi
{

enum class Difficulty
{
    easy,
    medium,
    hard
};

struct PlayerSelection
{
    PieceColor color;
    bool againstComputer;
    std::string fenString;
    Difficulty difficulty;
};

class GameMenu
{
public:
    GameMenu(sf::RenderWindow* window);
    /**
     * @return Selected player color, true if against computer and position fen string (can be
     * empty).
     */
    PlayerSelection runMenu();

private:
    void handleEvents();
    void handleWindowResize(const sf::Event& event);
    void handleMousePressed(const sf::Event& event);
    void handleMouseMoved(const sf::Event& event);
    void drawGameMenu();
    void drawSelectionOfColor();
    void drawSelectionOfOpponent();
    void drawFenText();
    void setPlayerColor(PieceColor color);
    void setAgainstComputer(bool againstComputer);
    void handlePlayerColor(const sf::Event& event);
    void handleComputerChoice(const sf::Event& event);
    void settingDifficultyStyle(const sf::Event& event);

private:
    sf::RenderWindow* m_window;
    sf::Text m_headlineColorText;
    sf::Text m_headlineOpponentText;
    sf::Text m_whitePlayerText;
    sf::Text m_blackPlayerText;
    sf::Text m_againstHumanText;
    sf::Text m_againstComputerText;
    sf::Text m_fenText;
    sf::Text m_easyText;
    sf::Text m_mediumText;
    sf::Text m_hardText;
    sf::Font m_font;
    std::optional<PieceColor> m_playerColor;
    std::optional<bool> m_againstComputer;
    std::optional<Difficulty> m_difficulty;
    std::string m_fenString;
};

} // namespace chessAi