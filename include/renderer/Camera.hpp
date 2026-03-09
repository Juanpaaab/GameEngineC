#pragma once
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

namespace engine {

class Camera {
public:
    Camera() = default;
    explicit Camera(float width, float height);

    void setPosition(sf::Vector2f pos);
    void move(sf::Vector2f delta);
    void setZoom(float zoom); // 1.0 = normal
    void setRotation(float degrees);

    sf::Vector2f getPosition()    const { return m_view.getCenter(); }
    float        getZoom()        const { return m_zoom; }
    float        getRotation()    const { return m_view.getRotation(); }
    const sf::View& getView()     const { return m_view; }

    // Follow a target smoothly
    void follow(sf::Vector2f target, float lerpFactor = 1.f);

private:
    sf::View m_view;
    float    m_zoom = 1.f;
};

} // namespace engine
