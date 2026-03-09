#pragma once
#include "ecs/World.hpp"

namespace engine {

class Game;

class Scene {
public:
    explicit Scene(Game& game) : m_game(game) {}
    virtual ~Scene() = default;

    virtual void onEnter()  {}
    virtual void onExit()   {}
    virtual void update(float dt);
    virtual void render();

    World& getWorld() { return m_world; }

protected:
    Game&  m_game;
    World  m_world;
};

} // namespace engine
