#pragma once
#include "renderer/Camera.hpp"
#include "ecs/World.hpp"
#include <SFML/Graphics.hpp>

namespace engine {

class Window;

class Renderer {
public:
    Renderer() = default;

    void setWindow(Window* window) { m_window = window; }

    // Render all entities in the world that have SpriteComponent / TextComponent
    void renderWorld(World& world);

    // Low-level draw helpers
    void drawSprite(const sf::Sprite& sprite);
    void drawText(const sf::Text& text);
    void drawShape(const sf::Shape& shape);
    void drawLine(sf::Vector2f a, sf::Vector2f b, sf::Color color, float thickness = 1.f);
    void drawRect(sf::FloatRect rect, sf::Color fill, sf::Color outline = sf::Color::Transparent, float outlineThickness = 0.f);
    void drawCircle(sf::Vector2f center, float radius, sf::Color fill, sf::Color outline = sf::Color::Transparent);

    Camera& getCamera() { return m_camera; }

private:
    Window* m_window = nullptr;
    Camera  m_camera;
};

} // namespace engine
