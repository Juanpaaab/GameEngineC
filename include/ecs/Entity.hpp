#pragma once
#include "ecs/Component.hpp"
#include <unordered_map>
#include <memory>
#include <string>

namespace engine {

class Entity {
public:
    explicit Entity(EntityID id, std::string name = "Entity");
    ~Entity() = default;

    EntityID           getID()   const { return m_id; }
    const std::string& getName() const { return m_name; }
    void               setName(std::string name) { m_name = std::move(name); }

    bool isActive() const  { return m_active; }
    void setActive(bool v) { m_active = v; }

    template <typename T, typename... Args>
    T& addComponent(Args&&... args) {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *comp;
        m_components[getComponentID<T>()] = std::move(comp);
        return ref;
    }

    template <typename T>
    T* getComponent() {
        auto it = m_components.find(getComponentID<T>());
        if (it == m_components.end()) return nullptr;
        return static_cast<T*>(it->second.get());
    }

    template <typename T>
    const T* getComponent() const {
        auto it = m_components.find(getComponentID<T>());
        if (it == m_components.end()) return nullptr;
        return static_cast<const T*>(it->second.get());
    }

    template <typename T>
    bool hasComponent() const {
        return m_components.count(getComponentID<T>()) > 0;
    }

    template <typename T>
    void removeComponent() {
        m_components.erase(getComponentID<T>());
    }

private:
    EntityID   m_id;
    std::string m_name;
    bool        m_active = true;
    std::unordered_map<ComponentID, std::unique_ptr<Component>> m_components;
};

} // namespace engine
