#pragma once
#include "tilemap/TileMap.hpp"
#include "data/PokemonData.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <vector>
#include <array>

namespace pokemon {

enum class Direction { Down=0, Left, Right, Up };

class Player {
public:
    Player();

    void setPosition(int tileX, int tileY);
    void update(float dt, const TileMap& map);
    void draw(sf::RenderTarget& target) const;

    // Tile coordinates
    sf::Vector2i getTilePos() const { return m_tilePos; }
    // Pixel-smooth position (for camera)
    sf::Vector2f getPixelPos() const { return m_pixelPos; }

    Direction    getDirection() const { return m_dir; }
    bool         isMoving()     const { return m_moving; }

    // Returns true if the player just stepped onto tall grass this frame
    bool justEnteredEncounterZone() const { return m_justEnteredEncounter; }

    // Party
    static const int MAX_PARTY = 6;
    std::array<PokemonInstance, MAX_PARTY> party;
    int partySize = 0;

    void addToParty(Species s, uint8_t level, const std::array<MoveID, PokemonInstance::MAX_MOVES>& moves);

    // Carga un spritesheet del personaje.
    // Formato esperado: 4 filas (Down/Left/Right/Up) x 3 columnas (stand/walk1/walk2),
    // cada frame de TILE_SIZE x TILE_SIZE píxeles.
    void loadTexture(const std::string& path);

    // Steps taken (for encounter rate calculation)
    int steps = 0;

private:
    sf::Vector2i m_tilePos;
    sf::Vector2f m_pixelPos;
    sf::Vector2f m_prevPixel;
    sf::Vector2f m_targetPixel;
    Direction    m_dir    = Direction::Down;
    bool         m_moving = false;
    float        m_moveProgress = 0.f;  // 0..1
    static constexpr float MOVE_SPEED = 6.f; // tiles per second

    bool m_justEnteredEncounter = false;

    sf::Texture m_texture;
    bool        m_textureLoaded = false;
    int         m_walkFrame     = 0;  // 0=stand, 1=walkA, 2=walkB

    sf::Vector2i m_dirDelta() const;
};

} // namespace pokemon
