#include "core/Time.hpp"

namespace engine {

void Time::update() {
    m_deltaTime = m_clock.restart().asSeconds();
    // Clamp to 50 ms so a frame hitch never explodes physics / movement.
    if (m_deltaTime > 0.05f) m_deltaTime = 0.05f;
    m_totalTime += m_deltaTime;

    ++m_frameCount;
    if (m_fpsClock.getElapsedTime().asSeconds() >= 1.f) {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_fpsClock.restart();
    }
}

int Time::getFPS() const {
    return m_fps;
}

} // namespace engine
