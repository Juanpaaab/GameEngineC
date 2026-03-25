#include "dialog/DialogSystem.hpp"

namespace pokemon {

DialogSystem::DialogSystem(const sf::Font& font) : m_font(font) {}

void DialogSystem::show(const std::string& text, std::function<void()> onDone) {
    // Clear queue and show this text directly
    while (!m_queue.empty()) m_queue.pop();
    m_onDone   = onDone;
    m_fullText = text;
    m_displayed.clear();
    m_charTimer = 0.f;
    m_done      = false;
    m_open      = true;
}

void DialogSystem::showQueue(std::vector<std::string> lines, std::function<void()> onDone) {
    while (!m_queue.empty()) m_queue.pop();
    m_onDone = onDone;
    for (auto& l : lines) m_queue.push(l);
    nextLine();
}

void DialogSystem::nextLine() {
    if (m_queue.empty()) {
        m_open = false;
        if (m_onDone) m_onDone();
        return;
    }
    m_fullText = m_queue.front();
    m_queue.pop();
    m_displayed.clear();
    m_charTimer = 0.f;
    m_done      = false;
    m_open      = true;
}

void DialogSystem::advance() {
    if (!m_open) return;
    if (!m_done) {
        // Skip animation: show full text immediately
        m_displayed = m_fullText;
        m_done      = true;
    } else {
        nextLine();
    }
}

void DialogSystem::update(float dt) {
    if (!m_open || m_done) return;
    m_charTimer += dt;
    while (m_charTimer >= CHAR_DELAY && m_displayed.size() < m_fullText.size()) {
        m_charTimer -= CHAR_DELAY;
        m_displayed += m_fullText[m_displayed.size()];
    }
    if (m_displayed.size() == m_fullText.size())
        m_done = true;
}

void DialogSystem::draw(sf::RenderTarget& target, sf::Vector2u screenSize) const {
    if (!m_open) return;

    const float boxH  = 64.f;
    const float pad   = 8.f;
    float boxY = static_cast<float>(screenSize.y) - boxH;

    // Background
    sf::RectangleShape bg(sf::Vector2f(static_cast<float>(screenSize.x), boxH));
    bg.setPosition({0.f, boxY});
    bg.setFillColor(sf::Color(240, 240, 240));
    bg.setOutlineColor(sf::Color::Black);
    bg.setOutlineThickness(2.f);
    target.draw(bg);

    // Text
    sf::Text text(m_font);
    text.setCharacterSize(12);
    text.setFillColor(sf::Color::Black);
    text.setString(m_displayed);
    text.setPosition({pad, boxY + pad});
    target.draw(text);

    // "Press A" arrow indicator when done
    if (m_done) {
        sf::Text arrow(m_font);
        arrow.setCharacterSize(12);
        arrow.setFillColor(sf::Color::Black);
        arrow.setString(">");
        arrow.setPosition({static_cast<float>(screenSize.x) - 20.f,
                          static_cast<float>(screenSize.y) - 20.f});
        target.draw(arrow);
    }
}

} // namespace pokemon
