#pragma once

#include "core/PieceType.h"

#include <SFML/Graphics.hpp>

#include <optional>

namespace chessAi
{

class GameMenu
{
public:
    GameMenu(sf::RenderWindow* window);
    /**
     * @return Selected player color, true if against computer and position fen string (can be
     * empty).
     */
    std::tuple<PieceColor, bool, std::string> runMenu();

private:
    void handleEvents();
    void handleMousePressed(const sf::Event& event);
    void handleMouseMoved(const sf::Event& event);

private:
    sf::RenderWindow* m_window;
    sf::Text m_headlineColorText;
    sf::Text m_whitePlayerText;
    sf::Text m_blackPlayerText;
    sf::Text m_headlineOpponentText;
    sf::Text m_againstHumanText;
    sf::Text m_againstComputerText;
    sf::Text m_fenText;
    sf::Font m_font;
    std::optional<PieceColor> m_playerColor;
    std::optional<bool> m_againstComputer;
    std::string m_fenString;
};

} // namespace chessAi