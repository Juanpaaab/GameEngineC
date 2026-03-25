#include "renderer/Camera.hpp"
#include <cmath>

namespace engine {

Camera::Camera(float width, float height) {
    m_view.setSize({width, height});
    m_view.setCenter({width / 2.f, height / 2.f});
}

void Camera::setPosition(sf::Vector2f pos) {
    m_view.setCenter(pos);
}

void Camera::move(sf::Vector2f delta) {
    m_view.move(delta);
}

void Camera::setZoom(float zoom) {
    m_zoom = zoom;
    m_view.setSize(m_view.getSize() / m_zoom * zoom);
    m_zoom = zoom;
}

void Camera::setRotation(float degrees) {
    m_view.setRotation(sf::degrees(degrees));
}

void Camera::follow(sf::Vector2f target, float lerpFactor) {
    sf::Vector2f current = m_view.getCenter();
    sf::Vector2f dir = target - current;
    m_view.setCenter(current + dir * lerpFactor);
}

} // namespace engine
