#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <queue>
#include <functional>

namespace pokemon {

class DialogSystem {
public:
    explicit DialogSystem(const sf::Font& font);

    void show(const std::string& text, std::function<void()> onDone = nullptr);
    void showQueue(std::vector<std::string> lines, std::function<void()> onDone = nullptr);

    void update(float dt);
    void draw(sf::RenderTarget& target, sf::Vector2u screenSize) const;

    bool isOpen()     const { return m_open; }
    void advance();  // called on A/Enter press

private:
    void nextLine();

    const sf::Font&  m_font;
    bool             m_open       = false;
    std::string      m_fullText;
    std::string      m_displayed;
    float            m_charTimer  = 0.f;
    static constexpr float CHAR_DELAY = 0.04f; // seconds per character
    bool             m_done       = false;      // all chars shown

    std::queue<std::string>      m_queue;
    std::function<void()>        m_onDone;
};

} // namespace pokemon
