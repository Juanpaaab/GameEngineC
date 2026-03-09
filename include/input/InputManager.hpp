#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>
#include <unordered_map>

namespace engine {

class InputManager {
public:
    InputManager() = default;

    // Called each frame before polling events
    void beginFrame();
    // Feed each window event here
    void handleEvent(const sf::Event& event);

    // Keyboard
    bool isKeyDown(sf::Keyboard::Key key)     const;
    bool isKeyPressed(sf::Keyboard::Key key)  const; // true only first frame
    bool isKeyReleased(sf::Keyboard::Key key) const; // true only first frame

    // Mouse buttons
    bool isMouseDown(sf::Mouse::Button btn)     const;
    bool isMousePressed(sf::Mouse::Button btn)  const;
    bool isMouseReleased(sf::Mouse::Button btn) const;

    // Mouse position (window-space)
    sf::Vector2i getMousePosition()  const { return m_mousePos; }
    sf::Vector2i getMouseDelta()     const { return m_mouseDelta; }
    float        getScrollDelta()    const { return m_scrollDelta; }

private:
    std::unordered_map<int, bool> m_keysCurrent;
    std::unordered_map<int, bool> m_keysPrev;
    std::unordered_map<int, bool> m_mouseCurrents;
    std::unordered_map<int, bool> m_mousePrev;

    sf::Vector2i m_mousePos   = {0, 0};
    sf::Vector2i m_mousePrev2 = {0, 0};
    sf::Vector2i m_mouseDelta = {0, 0};
    float        m_scrollDelta = 0.f;
};

} // namespace engine
