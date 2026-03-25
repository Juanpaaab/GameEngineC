#pragma once
#include "ecs/Component.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <functional>

namespace engine {

// ---- Transform ----
struct Transform : Component {
    sf::Vector2f position = {0.f, 0.f};
    sf::Vector2f scale    = {1.f, 1.f};
    float        rotation = 0.f; // degrees
};

// ---- Sprite ----
struct SpriteComponent : Component {
    sf::Sprite sprite;
    int        zOrder = 0;

    SpriteComponent() = default;
    explicit SpriteComponent(const sf::Texture& tex) : sprite(tex) {}
};

// ---- Velocity / Physics ----
struct Velocity : Component {
    sf::Vector2f linear  = {0.f, 0.f};
    float        angular = 0.f; // degrees per second
};

// ---- Collider (AABB) ----
struct BoxCollider : Component {
    sf::Vector2f size   = {32.f, 32.f};
    sf::Vector2f offset = {0.f, 0.f};
    bool         isTrigger = false;
};

// ---- Tag ----
struct Tag : Component {
    std::string value;
    explicit Tag(std::string t) : value(std::move(t)) {}
};

// ---- Script (generic update callback) ----
struct Script : Component {
    std::function<void(float dt)>      onUpdate;
    std::function<void(Entity& other)> onCollision; // fired when AABB overlap occurs
    explicit Script(std::function<void(float dt)> fn)
        : onUpdate(std::move(fn)) {};
};

// ---- Animation ----
struct AnimationComponent : Component {
    sf::IntRect  frameRect;
    int          frameCount    = 1;
    int          cols          = 0;   // columns per row in spritesheet (0 = single-row layout)
    int          currentFrame  = 0;
    float        frameTime     = 0.1f; // seconds per frame
    float        elapsed       = 0.f;
    bool         looping       = true;
};


// ---- Text ----
// SFML 3: sf::Text requires a font at construction, so we use a pointer
struct TextComponent : Component {
    sf::Text* text = nullptr;  // Lazy-initialized with font when needed
    int       zOrder = 0;
    
    ~TextComponent() {
        if (text) delete text;
    }
    
    // Allocate text with font (call this after getting a font reference)
    void init(const sf::Font& font) {
        if (text) delete text;
        text = new sf::Text(font);
    }
};
} // namespace engine
