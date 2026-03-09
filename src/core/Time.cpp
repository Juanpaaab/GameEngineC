#include "core/Time.hpp"

namespace engine {

void Time::update() {
    m_deltaTime = m_clock.restart().asSeconds();
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
