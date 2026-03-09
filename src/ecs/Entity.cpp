#include "ecs/Entity.hpp"

namespace engine {

Entity::Entity(EntityID id, std::string name)
    : m_id(id), m_name(std::move(name))
{}

} // namespace engine
