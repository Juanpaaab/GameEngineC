#include "core/Window.hpp"
#include <optional>

namespace engine {

Window::Window(const WindowConfig& config)
    : m_config(config)
    , m_window(sf::VideoMode({config.width, config.height}), config.title,
               sf::Style::Default)
{
    if (config.vsync)
        m_window.setVerticalSyncEnabled(true);
    else
        m_window.setFramerateLimit(config.fps);
}

void Window::pollEvents() {
    // Drain all pending SFML events from the queue.
    // Useful to flush stale input after a scene transition.
    while (m_window.pollEvent()) {}
}

std::optional<sf::Event> Window::pollEvent() {
    return m_window.pollEvent();
}

void Window::clear(sf::Color color) {
    m_window.clear(color);
}

void Window::display() {
    m_window.display();
}

bool Window::isOpen() const {
    return m_window.isOpen();
}

void Window::close() {
    m_window.close();
}

void Window::draw(const sf::Drawable& drawable) {
    m_window.draw(drawable);
}

void Window::setView(const sf::View& view) {
    m_window.setView(view);
}

sf::View Window::getView() const {
    return m_window.getView();
}

sf::View Window::getDefaultView() const {
    return m_window.getDefaultView();
}

sf::Vector2u Window::getSize() const {
    return m_window.getSize();
}

} // namespace engine
