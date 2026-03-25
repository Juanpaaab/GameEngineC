#include "input/InputManager.hpp"

namespace engine {

void InputManager::beginFrame() {
    m_keysPrev     = m_keysCurrent;
    m_mousePrev    = m_mouseCurrents;
    m_mousePrev2   = m_mousePos;
    m_scrollDelta  = 0.f;
    m_mouseDelta   = {0, 0};
}

void InputManager::handleEvent(const sf::Event& event) {
    if (const auto* e = event.getIf<sf::Event::KeyPressed>()) {
        m_keysCurrent[static_cast<int>(e->code)] = true;
    } else if (const auto* e = event.getIf<sf::Event::KeyReleased>()) {
        m_keysCurrent[static_cast<int>(e->code)] = false;
    } else if (const auto* e = event.getIf<sf::Event::MouseButtonPressed>()) {
        m_mouseCurrents[static_cast<int>(e->button)] = true;
    } else if (const auto* e = event.getIf<sf::Event::MouseButtonReleased>()) {
        m_mouseCurrents[static_cast<int>(e->button)] = false;
    } else if (const auto* e = event.getIf<sf::Event::MouseMoved>()) {
        m_mouseDelta = e->position - m_mousePos;
        m_mousePos   = e->position;
    } else if (const auto* e = event.getIf<sf::Event::MouseWheelScrolled>()) {
        m_scrollDelta = e->delta;
    }
}

bool InputManager::isKeyDown(sf::Keyboard::Key key) const {
    auto it = m_keysCurrent.find(static_cast<int>(key));
    return it != m_keysCurrent.end() && it->second;
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) const {
    int k = static_cast<int>(key);
    bool cur  = m_keysCurrent.count(k) && m_keysCurrent.at(k);
    bool prev = m_keysPrev.count(k)    && m_keysPrev.at(k);
    return cur && !prev;
}

bool InputManager::isKeyReleased(sf::Keyboard::Key key) const {
    int k = static_cast<int>(key);
    bool cur  = m_keysCurrent.count(k) && m_keysCurrent.at(k);
    bool prev = m_keysPrev.count(k)    && m_keysPrev.at(k);
    return !cur && prev;
}

bool InputManager::isMouseDown(sf::Mouse::Button btn) const {
    auto it = m_mouseCurrents.find(static_cast<int>(btn));
    return it != m_mouseCurrents.end() && it->second;
}

bool InputManager::isMousePressed(sf::Mouse::Button btn) const {
    int b = static_cast<int>(btn);
    bool cur  = m_mouseCurrents.count(b) && m_mouseCurrents.at(b);
    bool prev = m_mousePrev.count(b)      && m_mousePrev.at(b);
    return cur && !prev;
}

bool InputManager::isMouseReleased(sf::Mouse::Button btn) const {
    int b = static_cast<int>(btn);
    bool cur  = m_mouseCurrents.count(b) && m_mouseCurrents.at(b);
    bool prev = m_mousePrev.count(b)      && m_mousePrev.at(b);
    return !cur && prev;
}

} // namespace engine
