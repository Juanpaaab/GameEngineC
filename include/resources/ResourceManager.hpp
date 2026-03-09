#pragma once
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>

namespace engine {

template <typename T>
class ResourceCache {
public:
    T& load(const std::string& id, const std::string& path) {
        auto it = m_cache.find(id);
        if (it != m_cache.end()) return *it->second;

        auto res = std::make_unique<T>();
        if (!res->loadFromFile(path))
            throw std::runtime_error("Failed to load resource: " + path);

        T& ref = *res;
        m_cache[id] = std::move(res);
        return ref;
    }

    T* get(const std::string& id) {
        auto it = m_cache.find(id);
        return (it != m_cache.end()) ? it->second.get() : nullptr;
    }

    void unload(const std::string& id) { m_cache.erase(id); }
    void clear()                        { m_cache.clear(); }

private:
    std::unordered_map<std::string, std::unique_ptr<T>> m_cache;
};

class ResourceManager {
public:
    ResourceCache<sf::Texture>     textures;
    ResourceCache<sf::Font>        fonts;
    ResourceCache<sf::SoundBuffer> sounds;
};

} // namespace engine
