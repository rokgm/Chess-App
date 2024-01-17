#include "GameMenu.h"

namespace chessAi
{

namespace
{

unsigned int getSmallestNumber(unsigned int x, unsigned int y)
{
    return (x < y) ? x : y;
}

void setText(sf::Text& text, const sf::Color& color, const std::string& label, const sf::Font& font,
             const sf::RenderWindow* window, float characterScale)
{
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(static_cast<unsigned int>(
        characterScale *
        static_cast<float>(getSmallestNumber(window->getSize().x, window->getSize().y))));
    text.setFillColor(color);
    text.setStyle(sf::Text::Bold);
    text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
}

} // namespace

GameMenu::GameMenu(sf::RenderWindow* window)
    : m_window(window), m_headlineColorText(), m_whitePlayerText(), m_blackPlayerText(),
      m_headlineOpponentText(), m_againstHumanText(), m_againstComputerText(), m_fenText(),
      m_font(), m_playerColor(std::nullopt), m_againstComputer(std::nullopt), m_fenString("")
{

    if (!m_font.loadFromFile("fonts/arial_narrow_7.ttf")) {
        throw std::runtime_error("Game menu font couldn't be loaded.");
    }

    // Selection of your color
    setText(m_headlineColorText, sf::Color::White,
            "Please select the color \n     you wish to play", m_font, m_window, 0.08f);
    setText(m_whitePlayerText, sf::Color::White, "white", m_font, m_window, 0.05f);
    setText(m_blackPlayerText, sf::Color::White, "black", m_font, m_window, 0.05f);

    // Selection of your opponent
    setText(m_headlineOpponentText, sf::Color::White, "I want to play against a", m_font, m_window,
            0.08f);
    setText(m_againstHumanText, sf::Color::White, "human", m_font, m_window, 0.05f);
    setText(m_againstComputerText, sf::Color::White, "computer", m_font, m_window, 0.05f);

    // Fen text
    setText(m_fenText, sf::Color::White,
            "To start from different position\n  paste fen string to window", m_font, m_window,
            0.04f);
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

void GameMenu::handleMousePressed(const sf::Event& event)
{
    if (pressedInSquare(event.mouseButton, m_whitePlayerText.getGlobalBounds())) {
        m_playerColor = PieceColor::White;
    }
    else if (pressedInSquare(event.mouseButton, m_blackPlayerText.getGlobalBounds())) {
        m_playerColor = PieceColor::Black;
    }
    else if (pressedInSquare(event.mouseButton, m_againstHumanText.getGlobalBounds())) {
        m_againstComputer = false;
    }
    else if (pressedInSquare(event.mouseButton, m_againstComputerText.getGlobalBounds())) {
        m_againstComputer = true;
    }
}

void GameMenu::handleMouseMoved(const sf::Event& event)
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

    if (!m_againstComputer.has_value()) {
        if (touchedInSquare(event.mouseMove, m_againstHumanText.getGlobalBounds())) {
            m_againstHumanText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        }
        else if (touchedInSquare(event.mouseMove, m_againstComputerText.getGlobalBounds())) {
            m_againstComputerText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        }
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

void GameMenu::handleEvents()
{
    sf::Event event;
    while (m_window->pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            m_window->close();
            break;
        case sf::Event::Resized:
            m_window->setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(event.size.width),
                                                     static_cast<float>(event.size.height))));
            break;
        case sf::Event::MouseButtonPressed:
            handleMousePressed(event);
            break;
        case sf::Event::MouseMoved:
            handleMouseMoved(event);
            break;
        case sf::Event::KeyPressed:
            if (event.key.control && event.key.scancode == sf::Keyboard::Scan::V) {
                m_fenString = sf::Clipboard::getString();
                m_fenText.setStyle(sf::Text::Bold | sf::Text::Underlined);
            }
            break;
        default:
            break;
        }
    }
}

std::tuple<PieceColor, bool, std::string> GameMenu::runMenu()
{
    while (m_window->isOpen()) {
        handleEvents();

        if (m_playerColor.has_value() && m_againstComputer.has_value()) {
            return {*m_playerColor, *m_againstComputer, m_fenString};
        }

        // Set  grey background color.
        m_window->clear(sf::Color(50, 50, 50, 255));

        auto windowSize = static_cast<sf::Vector2f>(m_window->getSize());
        m_headlineColorText.setPosition(0.5f * windowSize.x, 0.1f * windowSize.y);
        m_whitePlayerText.setPosition(
            0.5f * windowSize -
            sf::Vector2f(0.f, 1.8f * m_headlineColorText.getLocalBounds().height));
        m_blackPlayerText.setPosition(
            0.5f * windowSize -
            sf::Vector2f(0.f, 1.8f * m_headlineColorText.getLocalBounds().height -
                                  2.f * m_whitePlayerText.getLocalBounds().height));
        m_headlineOpponentText.setPosition(0.5f * windowSize.x, 0.5f * windowSize.y);
        m_againstHumanText.setPosition(
            0.5f * windowSize -
            sf::Vector2f(0.f, -2.f * m_headlineOpponentText.getLocalBounds().height));
        m_againstComputerText.setPosition(
            0.5f * windowSize -
            sf::Vector2f(0.f, -2.f * m_headlineOpponentText.getLocalBounds().height -
                                  2.f * m_againstHumanText.getLocalBounds().height));
        m_fenText.setPosition(
            sf::Vector2f(0.5f * static_cast<float>(m_window->getSize().x),
                         m_againstComputerText.getGlobalBounds().top +
                             3.f * m_againstComputerText.getGlobalBounds().height));

        m_window->draw(m_headlineColorText);
        m_window->draw(m_whitePlayerText);
        m_window->draw(m_blackPlayerText);
        m_window->draw(m_headlineOpponentText);
        m_window->draw(m_againstHumanText);
        m_window->draw(m_againstComputerText);
        m_window->draw(m_fenText);
        m_window->display();
    }
    return {PieceColor::White, false, ""};
}

} // namespace chessAi