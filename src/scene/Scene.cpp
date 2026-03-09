#include "scene/Scene.hpp"
#include "core/Game.hpp"

namespace engine {

void Scene::update(float dt) {
    m_world.update(dt);
}

void Scene::render() {
    m_game.getRenderer().renderWorld(m_world);
}

} // namespace engine
