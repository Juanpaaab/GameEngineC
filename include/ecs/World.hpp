#pragma once
#include "ecs/Entity.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace engine {

class World {
public:
    World() = default;

    Entity&  createEntity(const std::string& name = "Entity");
    void     destroyEntity(EntityID id);
    Entity*  getEntity(EntityID id);
    bool     hasEntity(EntityID id) const;

    // Iterate over all active entities that have all listed components
    template <typename... Ts>
    void forEach(std::function<void(Entity&)> fn) {
        for (auto& [id, entity] : m_entities) {
            if (!entity->isActive()) continue;
            if ((entity->hasComponent<Ts>() && ...))
                fn(*entity);
        }
    }

    void update(float dt);
    void clear();

    std::size_t entityCount() const { return m_entities.size(); }

private:
    std::unordered_map<EntityID, std::unique_ptr<Entity>> m_entities;
    std::vector<EntityID>                                  m_toDestroy;
    EntityID                                               m_nextID = 1;
};

} // namespace engine
