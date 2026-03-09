#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace engine {

struct WindowConfig {
    unsigned int width  = 800;
    unsigned int height = 600;
    std::string  title  = "Game Engine 2D";
    unsigned int fps    = 60;
    bool         vsync  = false;
};

class Window {
public:
    explicit Window(const WindowConfig& config = {});
    ~Window() = default;

    void         pollEvents();
    void         clear(sf::Color color = sf::Color::Black);
    void         display();
    bool         isOpen() const;
    void         close();

    void         draw(const sf::Drawable& drawable);
    void         setView(const sf::View& view);
    sf::View     getView() const;
    sf::View     getDefaultView() const;

    unsigned int getWidth()  const { return m_config.width; }
    unsigned int getHeight() const { return m_config.height; }
    sf::Vector2u getSize()   const;

    sf::RenderWindow&       getRenderWindow()       { return m_window; }
    const sf::RenderWindow& getRenderWindow() const { return m_window; }

    // Last polled event (call pollEvents first)
    bool pollEvent(sf::Event& event);

private:
    WindowConfig     m_config;
    sf::RenderWindow m_window;
};

} // namespace engine
