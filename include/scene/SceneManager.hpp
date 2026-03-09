#pragma once
#include "scene/Scene.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace engine {

class Game;

class SceneManager {
public:
    using SceneFactory = std::function<std::unique_ptr<Scene>(Game&)>;

    explicit SceneManager(Game& game) : m_game(game) {}

    void registerScene(const std::string& name, SceneFactory factory);
    void switchTo(const std::string& name);
    void update(float dt);
    void render();

    Scene*       getCurrentScene()       { return m_current.get(); }
    const Scene* getCurrentScene() const { return m_current.get(); }

private:
    Game& m_game;
    std::unordered_map<std::string, SceneFactory> m_factories;
    std::unique_ptr<Scene>                         m_current;
    std::string                                    m_pending;
};

} // namespace engine
