#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace pokemon {

static constexpr int TILE_SIZE = 16;

// Tile IDs used in maps
enum class TileID : uint8_t {
    Grass     = 0,  // walkable
    TallGrass = 1,  // walkable + encounter
    Water     = 2,  // not walkable
    Tree      = 3,  // not walkable
    Wall      = 4,  // not walkable
    Path      = 5,  // walkable
    Sand      = 6,  // walkable
    FloorIn   = 7,  // indoor floor (walkable)
    Sign      = 8,  // interact
    Door      = 9,  // warp trigger
    Fence     = 10, // not walkable
    Flower    = 11, // walkable decoration
    COUNT
};

bool tileWalkable(TileID t);
bool tileHasEncounter(TileID t);
sf::Color tileColor(TileID t);

struct TileMap {
    int width  = 0;
    int height = 0;
    std::vector<TileID> tiles;       // [y*width + x]
    std::vector<uint8_t> collision;  // 0=walk, 1=block

    void create(int w, int h, const std::vector<TileID>& data);
    TileID get(int x, int y) const;
    bool isWalkable(int tileX, int tileY) const;
    bool isInBounds(int tileX, int tileY) const;

    // Render the visible portion of the map
    void draw(sf::RenderTarget& target, sf::Vector2i camTile, int viewW, int viewH) const;
};

} // namespace pokemon
