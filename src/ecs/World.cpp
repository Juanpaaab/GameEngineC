#include "ecs/World.hpp"
#include "ecs/Components.hpp"

namespace engine {

Entity& World::createEntity(const std::string& name) {
    EntityID id = m_nextID++;
    m_entities[id] = std::make_unique<Entity>(id, name);
    return *m_entities[id];
}

void World::destroyEntity(EntityID id) {
    m_toDestroy.push_back(id);
}

Entity* World::getEntity(EntityID id) {
    auto it = m_entities.find(id);
    return (it != m_entities.end()) ? it->second.get() : nullptr;
}

bool World::hasEntity(EntityID id) const {
    return m_entities.count(id) > 0;
}

void World::update(float dt) {
    // Process script components
    forEach<Script>([dt](Entity& e) {
        auto* script = e.getComponent<Script>();
        if (script && script->onUpdate)
            script->onUpdate(dt);
    });

    // Simple physics: apply velocity to transform
    forEach<Transform, Velocity>([dt](Entity& e) {
        auto& t = *e.getComponent<Transform>();
        auto& v = *e.getComponent<Velocity>();
        t.position += v.linear * dt;
        t.rotation += v.angular * dt;
    });

    // Apply transform to sprite
    forEach<Transform, SpriteComponent>([](Entity& e) {
        auto& t = *e.getComponent<Transform>();
        auto& s = *e.getComponent<SpriteComponent>();
        s.sprite.setPosition(t.position);
        s.sprite.setRotation(t.rotation);
        s.sprite.setScale(t.scale);
    });

    // Apply transform to text
    forEach<Transform, TextComponent>([](Entity& e) {
        auto& t = *e.getComponent<Transform>();
        auto& tc = *e.getComponent<TextComponent>();
        tc.text.setPosition(t.position);
        tc.text.setRotation(t.rotation);
    });

    // Animate sprites
    forEach<SpriteComponent, AnimationComponent>([dt](Entity& e) {
        auto& anim = *e.getComponent<AnimationComponent>();
        auto& sc   = *e.getComponent<SpriteComponent>();

        anim.elapsed += dt;
        if (anim.elapsed >= anim.frameTime) {
            anim.elapsed -= anim.frameTime;
            anim.currentFrame++;
            if (anim.currentFrame >= anim.frameCount) {
                anim.currentFrame = anim.looping ? 0 : anim.frameCount - 1;
            }
            sf::IntRect r = anim.frameRect;
            r.left = r.width * anim.currentFrame;
            sc.sprite.setTextureRect(r);
        }
    });

    // Flush destroyed entities
    for (EntityID id : m_toDestroy)
        m_entities.erase(id);
    m_toDestroy.clear();
}

void World::clear() {
    m_entities.clear();
    m_toDestroy.clear();
}

} // namespace engine
