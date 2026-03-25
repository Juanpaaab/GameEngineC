#include "player/Player.hpp"
#include <SFML/Window/Keyboard.hpp>

namespace pokemon {

Player::Player() {
    m_tilePos     = {0, 0};
    m_pixelPos    = {0.f, 0.f};
    m_targetPixel = m_pixelPos;
    m_prevPixel   = m_pixelPos;
}

void Player::setPosition(int tx, int ty) {
    m_tilePos     = {tx, ty};
    m_pixelPos    = sf::Vector2f(static_cast<float>(tx * TILE_SIZE),
                                 static_cast<float>(ty * TILE_SIZE));
    m_targetPixel = m_pixelPos;
    m_prevPixel   = m_pixelPos;
    m_moving      = false;
}

sf::Vector2i Player::m_dirDelta() const {
    switch(m_dir) {
    case Direction::Up:    return {0,-1};
    case Direction::Down:  return {0, 1};
    case Direction::Left:  return {-1,0};
    case Direction::Right: return {1, 0};
    }
    return {0,0};
}

void Player::update(float dt, const TileMap& map) {
    m_justEnteredEncounter = false;

    if (m_moving) {
        m_moveProgress += MOVE_SPEED * dt;
        if (m_moveProgress >= 1.f) {
            m_moveProgress = 1.f;
            m_moving       = false;
            m_pixelPos     = m_targetPixel;

            TileID tile = map.get(m_tilePos.x, m_tilePos.y);
            if (tileHasEncounter(tile)) {
                ++steps;
                if (steps % 5 == 0)
                    m_justEnteredEncounter = true;
            }
        } else {
            m_pixelPos = m_prevPixel + (m_targetPixel - m_prevPixel) * m_moveProgress;
        }
        return;
    }

    sf::Vector2i delta = {0, 0};
    if      (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)    || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    { m_dir = Direction::Up;    delta = { 0,-1}; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)  || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    { m_dir = Direction::Down;  delta = { 0, 1}; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)  || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    { m_dir = Direction::Left;  delta = {-1, 0}; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    { m_dir = Direction::Right; delta = { 1, 0}; }

    if (delta != sf::Vector2i{0,0}) {
        sf::Vector2i next = m_tilePos + delta;
        if (map.isWalkable(next.x, next.y)) {
            m_prevPixel   = m_pixelPos;
            m_tilePos     = next;
            m_targetPixel = sf::Vector2f(static_cast<float>(next.x * TILE_SIZE),
                                         static_cast<float>(next.y * TILE_SIZE));
            m_moving      = true;
            m_moveProgress= 0.f;
        }
    }
}

void Player::draw(sf::RenderTarget& target) const {
    sf::RectangleShape body(sf::Vector2f(TILE_SIZE - 2.f, TILE_SIZE - 2.f));
    body.setPosition(m_pixelPos + sf::Vector2f(1.f, 1.f));
    body.setFillColor(sf::Color(230, 100, 100));

    sf::RectangleShape eye(sf::Vector2f(4.f, 4.f));
    sf::Vector2f eyeOffset;
    switch(m_dir) {
    case Direction::Up:    eyeOffset = {6.f,  1.f}; break;
    case Direction::Down:  eyeOffset = {6.f,  9.f}; break;
    case Direction::Left:  eyeOffset = {1.f,  6.f}; break;
    case Direction::Right: eyeOffset = {9.f,  6.f}; break;
    }
    eye.setPosition(m_pixelPos + eyeOffset);
    eye.setFillColor(sf::Color::White);

    target.draw(body);
    target.draw(eye);
}

void Player::addToParty(Species s, uint8_t level, const std::array<MoveID, PokemonInstance::MAX_MOVES>& moves) {
    if (partySize >= MAX_PARTY) return;
    party[partySize++].init(s, level, moves);
}

} // namespace pokemon
