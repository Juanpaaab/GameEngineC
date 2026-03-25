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
        s.sprite.setRotation(sf::degrees(t.rotation));
        s.sprite.setScale(t.scale);
    });


    // Apply transform to text
    forEach<Transform, TextComponent>([](Entity& e) {
        auto& t = *e.getComponent<Transform>();
        auto& tc = *e.getComponent<TextComponent>();
        if (tc.text) {
            tc.text->setPosition(t.position);
            tc.text->setRotation(sf::degrees(t.rotation));
        }
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
            // Multi-row spritesheet aware: use cols to determine row/column.
            // If cols == 0, treat all frames as a single horizontal row.
            int effectiveCols = (anim.cols > 0) ? anim.cols : anim.frameCount;
            int col = anim.currentFrame % effectiveCols;
            int row = anim.currentFrame / effectiveCols;
            sf::IntRect r = anim.frameRect;
            // SFML 3: Rect uses .position and .size (not .left/.top/.width/.height)
            r.position.x = anim.frameRect.position.x + col * anim.frameRect.size.x;
            r.position.y = anim.frameRect.position.y + row * anim.frameRect.size.y;
            sc.sprite.setTextureRect(r);
        }
    });

    // AABB collision detection
    std::vector<Entity*> colEntities;
    forEach<Transform, BoxCollider>([&colEntities](Entity& e) {
        colEntities.push_back(&e);
    });
    for (std::size_t i = 0; i < colEntities.size(); ++i) {
        for (std::size_t j = i + 1; j < colEntities.size(); ++j) {
            Entity* a = colEntities[i];
            Entity* b = colEntities[j];
            auto& ta = *a->getComponent<Transform>();
            auto& ca = *a->getComponent<BoxCollider>();
            auto& tb = *b->getComponent<Transform>();
            auto& cb = *b->getComponent<BoxCollider>();

            sf::Vector2f pa = ta.position + ca.offset;
            sf::Vector2f pb = tb.position + cb.offset;

            bool overlapX = pa.x < pb.x + cb.size.x && pa.x + ca.size.x > pb.x;
            bool overlapY = pa.y < pb.y + cb.size.y && pa.y + ca.size.y > pb.y;
            if (!overlapX || !overlapY) continue;

            if (!ca.isTrigger && !cb.isTrigger) {
                // Solid collision: resolve along the axis with smallest overlap
                float ox = (pa.x < pb.x)
                    ? pa.x + ca.size.x - pb.x
                    : pa.x - (pb.x + cb.size.x);
                float oy = (pa.y < pb.y)
                    ? pa.y + ca.size.y - pb.y
                    : pa.y - (pb.y + cb.size.y);
                if (std::abs(ox) <= std::abs(oy))
                    ta.position.x -= ox;
                else
                    ta.position.y -= oy;
            }
            // Fire optional collision callbacks
            if (auto* sa = a->getComponent<Script>(); sa && sa->onCollision)
                sa->onCollision(*b);
            if (auto* sb = b->getComponent<Script>(); sb && sb->onCollision)
                sb->onCollision(*a);
        }
    }

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
