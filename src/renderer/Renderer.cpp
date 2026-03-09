#include "renderer/Renderer.hpp"
#include "core/Window.hpp"
#include "ecs/Components.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace engine {

void Renderer::renderWorld(World& world) {
    if (!m_window) return;

    m_window->setView(m_camera.getView());

    // Collect sprites sorted by zOrder
    std::vector<std::pair<int, sf::Sprite*>> sprites;
    world.forEach<SpriteComponent>([&](Entity& e) {
        auto* sc = e.getComponent<SpriteComponent>();
        sprites.emplace_back(sc->zOrder, &sc->sprite);
    });
    std::sort(sprites.begin(), sprites.end(),
              [](const auto& a, const auto& b){ return a.first < b.first; });
    for (auto& [z, s] : sprites)
        m_window->draw(*s);

    // Collect text sorted by zOrder
    std::vector<std::pair<int, sf::Text*>> texts;
    world.forEach<TextComponent>([&](Entity& e) {
        auto* tc = e.getComponent<TextComponent>();
        texts.emplace_back(tc->zOrder, &tc->text);
    });
    std::sort(texts.begin(), texts.end(),
              [](const auto& a, const auto& b){ return a.first < b.first; });
    for (auto& [z, t] : texts)
        m_window->draw(*t);

    // Reset to default view after rendering world
    m_window->setView(m_window->getDefaultView());
}

void Renderer::drawSprite(const sf::Sprite& sprite) {
    if (m_window) m_window->draw(sprite);
}

void Renderer::drawText(const sf::Text& text) {
    if (m_window) m_window->draw(text);
}

void Renderer::drawShape(const sf::Shape& shape) {
    if (m_window) m_window->draw(shape);
}

void Renderer::drawLine(sf::Vector2f a, sf::Vector2f b, sf::Color color, float thickness) {
    if (!m_window) return;
    sf::Vector2f dir = b - a;
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length == 0.f) return;

    sf::RectangleShape line(sf::Vector2f(length, thickness));
    line.setPosition(a);
    line.setFillColor(color);
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f;
    line.setRotation(angle);
    m_window->draw(line);
}

void Renderer::drawRect(sf::FloatRect rect, sf::Color fill, sf::Color outline, float outlineThickness) {
    if (!m_window) return;
    sf::RectangleShape shape(sf::Vector2f(rect.width, rect.height));
    shape.setPosition(rect.left, rect.top);
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
    shape.setOutlineThickness(outlineThickness);
    m_window->draw(shape);
}

void Renderer::drawCircle(sf::Vector2f center, float radius, sf::Color fill, sf::Color outline) {
    if (!m_window) return;
    sf::CircleShape shape(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(center);
    shape.setFillColor(fill);
    shape.setOutlineColor(outline);
    m_window->draw(shape);
}

} // namespace engine
