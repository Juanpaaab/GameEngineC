#include "tilemap/TileMap.hpp"

namespace pokemon {

bool tileWalkable(TileID t) {
    switch(t) {
    case TileID::Water:
    case TileID::Tree:
    case TileID::Wall:
    case TileID::Fence:
        return false;
    default:
        return true;
    }
}

bool tileHasEncounter(TileID t) {
    return t == TileID::TallGrass;
}

sf::Color tileColor(TileID t) {
    switch(t) {
    case TileID::Grass:     return {120, 200,  80};
    case TileID::TallGrass: return { 60, 160,  40};
    case TileID::Water:     return { 80, 140, 220};
    case TileID::Tree:      return { 40, 100,  40};
    case TileID::Wall:      return {160, 120,  80};
    case TileID::Path:      return {200, 180, 140};
    case TileID::Sand:      return {220, 200, 120};
    case TileID::FloorIn:   return {220, 200, 160};
    case TileID::Sign:      return {180, 140,  80};
    case TileID::Door:      return {160,  90,  40};
    case TileID::Fence:     return {160, 100,  40};
    case TileID::Flower:    return {255, 180, 100};
    default:                return {100, 100, 100};
    }
}

void TileMap::create(int w, int h, const std::vector<TileID>& data) {
    width  = w;
    height = h;
    tiles  = data;
    collision.resize(w * h);
    for (int i = 0; i < w * h; ++i)
        collision[i] = tileWalkable(tiles[i]) ? 0 : 1;
}

TileID TileMap::get(int x, int y) const {
    if (!isInBounds(x, y)) return TileID::Wall;
    return tiles[y * width + x];
}

bool TileMap::isWalkable(int tx, int ty) const {
    if (!isInBounds(tx, ty)) return false;
    return collision[ty * width + tx] == 0;
}

bool TileMap::isInBounds(int tx, int ty) const {
    return tx >= 0 && ty >= 0 && tx < width && ty < height;
}

void TileMap::draw(sf::RenderTarget& target, sf::Vector2i camTile, int viewW, int viewH) const {
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    sf::RectangleShape border(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(0,0,0,30));
    border.setOutlineThickness(-1.f);

    for (int dy = -1; dy <= viewH + 1; ++dy) {
        for (int dx = -1; dx <= viewW + 1; ++dx) {
            int tx = camTile.x + dx;
            int ty = camTile.y + dy;
            TileID id = get(tx, ty);
            tile.setFillColor(tileColor(id));
            tile.setPosition({
                static_cast<float>(dx * TILE_SIZE),
                static_cast<float>(dy * TILE_SIZE)
            });
            target.draw(tile);

            // Tall grass gets a darker outline
            if (id == TileID::TallGrass) {
                border.setPosition(tile.getPosition());
                target.draw(border);
            }
        }
    }
}

} // namespace pokemon
