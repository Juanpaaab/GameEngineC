#include "scene/SceneManager.hpp"
#include "scene/Scene.hpp"

namespace engine {

void SceneManager::registerScene(const std::string& name, SceneFactory factory) {
    m_factories[name] = std::move(factory);
}

void SceneManager::switchTo(const std::string& name) {
    m_pending = name;
}

void SceneManager::update(float dt) {
    // Perform pending scene switch
    if (!m_pending.empty()) {
        if (m_current) m_current->onExit();

        auto it = m_factories.find(m_pending);
        if (it != m_factories.end()) {
            m_current = it->second(m_game);
            m_current->onEnter();
        }
        m_pending.clear();
    }

    if (m_current)
        m_current->update(dt);
}

void SceneManager::render() {
    if (m_current)
        m_current->render();
}

} // namespace engine
