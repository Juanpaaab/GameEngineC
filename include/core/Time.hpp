
#pragma once
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
namespace engine {

class Time {
public:
    Time() = default;

    void  update();
    float getDeltaTime()  const { return m_deltaTime; }
    float getTotalTime()  const { return m_totalTime; }
    int   getFPS()        const;

private:
    sf::Clock m_clock;
    sf::Clock m_fpsClock;
    float     m_deltaTime = 0.f;
    float     m_totalTime = 0.f;
    int       m_frameCount = 0;
    int       m_fps = 0;
};

} // namespace engine
