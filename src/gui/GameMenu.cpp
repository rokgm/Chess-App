#include "GameMenu.h"

namespace chessAi
{

namespace
{

void setText(sf::Text& text, const sf::Color& color, const std::string& label, const sf::Font& font,
             const sf::RenderWindow* window, float characterScale)
{
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(static_cast<unsigned int>(
        characterScale * static_cast<float>(std::min(window->getSize().x, window->getSize().y))));
    text.setFillColor(color);
    text.setStyle(sf::Text::Bold);
    text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
}

void resizeText(sf::Text& text, float characterScale, float positionX, float positionY,
                const sf::RenderWindow* window)
{
    text.setCharacterSize(static_cast<unsigned int>(
        characterScale * static_cast<float>(std::min(window->getSize().x, window->getSize().y))));
    text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
    text.setPosition(positionX, positionY);
}

} // namespace

GameMenu::GameMenu(sf::RenderWindow* window)
    : m_window(window), m_headlineColorText(), m_headlineOpponentText(), m_whitePlayerText(),
      m_blackPlayerText(), m_againstHumanText(), m_againstComputerText(), m_fenText(), m_easyText(),
      m_mediumText(), m_hardText(), m_font(), m_playerColor(std::nullopt),
      m_againstComputer(std::nullopt), m_difficulty(std::nullopt), m_fenString("")
{

    if (!m_font.loadFromFile("fonts/arial_narrow_7.ttf")) {
        throw std::runtime_error("Game menu font couldn't be loaded.");
    }

    // Selection of your color
    setText(m_headlineColorText, sf::Color::White,
            "Please select the color \n     you wish to play", m_font, m_window, 0.06f);
    setText(m_whitePlayerText, sf::Color::White, "white", m_font, m_window, 0.04f);
    setText(m_blackPlayerText, sf::Color::White, "black", m_font, m_window, 0.04f);

    // Selection of your opponent
    setText(m_headlineOpponentText, sf::Color::White, "I want to play against a", m_font, m_window,
            0.06f);
    setText(m_againstHumanText, sf::Color::White, "human", m_font, m_window, 0.04f);
    setText(m_againstComputerText, sf::Color::White, "computer", m_font, m_window, 0.04f);
    setText(m_easyText, sf::Color::White, "( easy", m_font, m_window, 0.03f);
    setText(m_mediumText, sf::Color::White, " medium ", m_font, m_window, 0.03f);
    setText(m_hardText, sf::Color::White, "hard )", m_font, m_window, 0.03f);

    // Fen text
    setText(m_fenText, sf::Color::White,
            "To start from a specific position paste \n  or type a valid fen string to window",
            m_font, m_window, 0.04f);
    m_fenText.setPosition(0.5f * static_cast<float>(m_window->getSize().x),
                          0.7f * static_cast<float>(m_window->getSize().y));
}

namespace
{

bool pressedInSquare(const sf::Event::MouseButtonEvent& mouse, const sf::FloatRect& bounds)
{
    return bounds.left < static_cast<float>(mouse.x) &&
           static_cast<float>(mouse.x) < bounds.left + bounds.width &&
           bounds.top < static_cast<float>(mouse.y) &&
           static_cast<float>(mouse.y) < bounds.top + bounds.height;
}

bool touchedInSquare(const sf::Event::MouseMoveEvent& mouse, const sf::FloatRect& bounds)
{
    return bounds.left < static_cast<float>(mouse.x) &&
           static_cast<float>(mouse.x) < bounds.left + bounds.width &&
           bounds.top < static_cast<float>(mouse.y) &&
           static_cast<float>(mouse.y) < bounds.top + bounds.height;
}

} // namespace

void GameMenu::handleWindowResize(const sf::Event& event)
{
    m_window->setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                             static_cast<float>(event.size.height))));
}

void GameMenu::setPlayerColor(PieceColor color)
{
    if (m_playerColor.has_value() && m_playerColor.value() == color)
        m_playerColor = std::nullopt;
    else
        m_playerColor = color;
}

void GameMenu::setAgainstComputer(bool againstComputer)
{
    if (m_againstComputer.has_value() && m_againstComputer.value() == againstComputer)
        m_againstComputer = std::nullopt;
    else
        m_againstComputer = againstComputer;
}

void GameMenu::handleMousePressed(const sf::Event& event)
{
    if (pressedInSquare(event.mouseButton, m_whitePlayerText.getGlobalBounds())) {
        setPlayerColor(PieceColor::White);
    }
    else if (pressedInSquare(event.mouseButton, m_blackPlayerText.getGlobalBounds())) {
        setPlayerColor(PieceColor::Black);
    }
    else if (pressedInSquare(event.mouseButton, m_againstHumanText.getGlobalBounds())) {
        setAgainstComputer(false);
        m_difficulty = std::nullopt;
    }
    else if (pressedInSquare(event.mouseButton, m_againstComputerText.getGlobalBounds())) {
        setAgainstComputer(true);
    }

    if (m_againstComputer.has_value() && m_againstComputer.value()) {
        if (pressedInSquare(event.mouseButton, m_easyText.getGlobalBounds())) {
            m_difficulty = Difficulty::easy;
        }
        else if (pressedInSquare(event.mouseButton, m_mediumText.getGlobalBounds())) {
            m_difficulty = Difficulty::medium;
        }
        else if (pressedInSquare(event.mouseButton, m_hardText.getGlobalBounds())) {
            m_difficulty = Difficulty::hard;
        }
    }
}

void GameMenu::settingDifficultyStyle(const sf::Event& event)
{
    if (!m_difficulty.has_value()) {
        if (touchedInSquare(event.mouseMove, m_easyText.getGlobalBounds()))
            m_easyText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        else if (touchedInSquare(event.mouseMove, m_mediumText.getGlobalBounds()))
            m_mediumText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        else if (touchedInSquare(event.mouseMove, m_hardText.getGlobalBounds()))
            m_hardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        else {
            m_easyText.setStyle(sf::Text::Bold);
            m_mediumText.setStyle(sf::Text::Bold);
            m_hardText.setStyle(sf::Text::Bold);
        }
    }
    else {
        if (m_difficulty.value() == Difficulty::easy) {
            m_easyText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            if (touchedInSquare(event.mouseMove, m_mediumText.getGlobalBounds()))
                m_mediumText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_mediumText.setStyle(sf::Text::Bold);
            if (touchedInSquare(event.mouseMove, m_hardText.getGlobalBounds()))
                m_hardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_hardText.setStyle(sf::Text::Bold);
        }
        else if (m_difficulty.value() == Difficulty::medium) {
            m_mediumText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            if (touchedInSquare(event.mouseMove, m_easyText.getGlobalBounds()))
                m_easyText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_easyText.setStyle(sf::Text::Bold);
            if (touchedInSquare(event.mouseMove, m_hardText.getGlobalBounds()))
                m_hardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_hardText.setStyle(sf::Text::Bold);
        }
        else {
            m_hardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            if (touchedInSquare(event.mouseMove, m_easyText.getGlobalBounds()))
                m_easyText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_easyText.setStyle(sf::Text::Bold);
            if (touchedInSquare(event.mouseMove, m_mediumText.getGlobalBounds()))
                m_mediumText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_mediumText.setStyle(sf::Text::Bold);
        }
    }
}

void GameMenu::handlePlayerColor(const sf::Event& event)
{
    if (!m_playerColor.has_value()) {
        if (touchedInSquare(event.mouseMove, m_whitePlayerText.getGlobalBounds())) {
            m_whitePlayerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        }
        else if (touchedInSquare(event.mouseMove, m_blackPlayerText.getGlobalBounds())) {
            m_blackPlayerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        }
        else {
            m_whitePlayerText.setStyle(sf::Text::Bold);
            m_blackPlayerText.setStyle(sf::Text::Bold);
        }
    }
    else {
        if (m_playerColor.value() == PieceColor::White) {
            m_whitePlayerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            if (touchedInSquare(event.mouseMove, m_blackPlayerText.getGlobalBounds()))
                m_blackPlayerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_blackPlayerText.setStyle(sf::Text::Bold);
        }
        else {
            m_blackPlayerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            if (touchedInSquare(event.mouseMove, m_whitePlayerText.getGlobalBounds()))
                m_whitePlayerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_whitePlayerText.setStyle(sf::Text::Bold);
        }
    }
}

void GameMenu::handleComputerChoice(const sf::Event& event)
{
    if (!m_againstComputer.has_value()) {
        if (touchedInSquare(event.mouseMove, m_againstHumanText.getGlobalBounds()))
            m_againstHumanText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        else if (touchedInSquare(event.mouseMove, m_againstComputerText.getGlobalBounds()))
            m_againstComputerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        else {
            m_againstHumanText.setStyle(sf::Text::Bold);
            m_againstComputerText.setStyle(sf::Text::Bold);
        }
    }
    else {
        if (m_againstComputer.value()) {
            m_againstComputerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            if (touchedInSquare(event.mouseMove, m_againstHumanText.getGlobalBounds()))
                m_againstHumanText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_againstHumanText.setStyle(sf::Text::Bold);

            settingDifficultyStyle(event);
        }
        else {
            m_againstHumanText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            if (touchedInSquare(event.mouseMove, m_againstComputerText.getGlobalBounds()))
                m_againstComputerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            else
                m_againstComputerText.setStyle(sf::Text::Bold);
        }
    }
}

void GameMenu::handleMouseMoved(const sf::Event& event)
{
    handlePlayerColor(event);
    handleComputerChoice(event);
}

void GameMenu::handleEvents()
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
            handleMousePressed(event);
            break;
        case sf::Event::MouseMoved:
            handleMouseMoved(event);
            break;
        case sf::Event::KeyPressed:
            if (event.key.control && event.key.code == sf::Keyboard::V) {
                m_fenString = sf::Clipboard::getString();
            }
            else if (event.key.code == sf::Keyboard::Backspace) {
                if (m_fenString.length() > 0)
                    m_fenString.pop_back();
            }
            break;
        case sf::Event::TextEntered:
            // The unicode of backspace is 8
            if (event.text.unicode != 8)
                m_fenString.push_back(static_cast<char>(event.text.unicode));
            break;
        default:
            break;
        }
    }
}

void GameMenu::drawGameMenu()
{
    // Set  grey background color.
    m_window->clear(sf::Color(50, 50, 50, 255));
    drawSelectionOfColor();
    drawSelectionOfOpponent();
    drawFenText();
}

void GameMenu::drawSelectionOfColor()
{
    resizeText(m_headlineColorText, 0.06f, 0.5f * static_cast<float>(m_window->getSize().x),
               0.1f * static_cast<float>(m_window->getSize().y), m_window);
    resizeText(m_whitePlayerText, 0.04f, m_headlineColorText.getPosition().x,
               m_headlineColorText.getPosition().y + m_headlineColorText.getLocalBounds().height,
               m_window);
    resizeText(m_blackPlayerText, 0.04f, m_whitePlayerText.getPosition().x,
               m_whitePlayerText.getPosition().y + 2.f * m_whitePlayerText.getLocalBounds().height,
               m_window);
    m_window->draw(m_headlineColorText);
    m_window->draw(m_whitePlayerText);
    m_window->draw(m_blackPlayerText);
}

void GameMenu::drawSelectionOfOpponent()
{
    resizeText(m_headlineOpponentText, 0.06f, 0.5f * static_cast<float>(m_window->getSize().x),
               0.4f * static_cast<float>(m_window->getSize().y), m_window);
    resizeText(m_againstHumanText, 0.04f, m_headlineOpponentText.getPosition().x,
               m_headlineOpponentText.getPosition().y +
                   1.2f * m_headlineOpponentText.getLocalBounds().height,
               m_window);
    resizeText(m_againstComputerText, 0.04f,
               m_againstHumanText.getPosition().x + (m_againstComputerText.getGlobalBounds().width -
                                                     m_againstHumanText.getGlobalBounds().width) /
                                                        2,
               m_againstHumanText.getPosition().y +
                   2.f * m_againstHumanText.getLocalBounds().height,
               m_window);
    resizeText(m_easyText, 0.03f,
               m_againstComputerText.getPosition().x +
                   1.2f * m_againstComputerText.getLocalBounds().width,
               m_againstComputerText.getPosition().y + 10.f, m_window);
    resizeText(m_mediumText, 0.03f,
               m_easyText.getPosition().x + 1.2f * m_easyText.getLocalBounds().width,
               m_easyText.getPosition().y, m_window);
    resizeText(m_hardText, 0.03f,
               m_mediumText.getPosition().x + 1.2f * m_mediumText.getLocalBounds().width,
               m_mediumText.getPosition().y, m_window);

    m_easyText.setOrigin(m_againstComputerText.getLocalBounds().width / 2,
                         m_againstComputerText.getLocalBounds().height / 2);
    m_mediumText.setOrigin(m_againstComputerText.getLocalBounds().width / 2,
                           m_againstComputerText.getLocalBounds().height / 2);
    m_hardText.setOrigin(m_againstComputerText.getLocalBounds().width / 2,
                         m_againstComputerText.getLocalBounds().height / 2);
    m_window->draw(m_headlineOpponentText);
    m_window->draw(m_againstHumanText);
    m_window->draw(m_againstComputerText);

    if (m_againstComputer.has_value() && m_againstComputer.value()) {
        m_window->draw(m_easyText);
        m_window->draw(m_mediumText);
        m_window->draw(m_hardText);
    }
}

void GameMenu::drawFenText()
{
    resizeText(m_fenText, 0.04f, 0.5f * static_cast<float>(m_window->getSize().x),
               0.7f * static_cast<float>(m_window->getSize().y), m_window);
    m_window->draw(m_fenText);

    if (m_fenString != "") {
        sf::Text fenStringToText;
        setText(fenStringToText, sf::Color::White, m_fenString, m_font, m_window, 0.03f);
        fenStringToText.setOrigin(m_fenText.getLocalBounds().width / 2,
                                  m_fenText.getLocalBounds().height / 2);
        fenStringToText.setPosition(0.5f * static_cast<float>(m_window->getSize().x),
                                    0.7f * static_cast<float>(m_window->getSize().y) +
                                        m_fenText.getLocalBounds().height);
        fenStringToText.setStyle(sf::Style::None);

        m_window->draw(fenStringToText);
    }
}

PlayerSelection GameMenu::runMenu()
{
    while (m_window->isOpen()) {
        handleEvents();
        drawGameMenu();

        if (m_playerColor.has_value() && m_againstComputer.has_value()) {
            if (*m_againstComputer && m_difficulty.has_value())
                return {*m_playerColor, *m_againstComputer, m_fenString, *m_difficulty};
            else if (!*m_againstComputer)
                return {*m_playerColor, *m_againstComputer, m_fenString, Difficulty::medium};
        }

        m_window->display();
    }
    return {PieceColor::White, false, "", Difficulty::medium};
}

} // namespace chessAi