#include "scenes/OverworldScene.hpp"
#include "scenes/BattleScene.hpp"
#include "core/Game.hpp"
#include <cstdlib>

namespace pokemon {

OverworldScene::OverworldScene(engine::Game& game,
                               PokemonInstance* party, int partySize,
                               PokemonInstance& wildPokemon)
    : engine::Scene(game)
    , m_party(party)
    , m_partySize(partySize)
    , m_wildPokemon(wildPokemon)
{}

void OverworldScene::onEnter() {
    m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");

    if (m_fontLoaded)
        m_dialog = std::make_unique<DialogSystem>(m_font);

    buildPalletTown();

    m_player.setPosition(5, 9);
    // Party is managed by PokemonGame; no need to re-add pokemon here

    if (m_dialog) {
        m_dialog->showQueue({
            "Welcome to PALLET TOWN!",
            "Walk into the tall grass to find wild Pokemon.",
            "Press Z / Enter to interact with signs.",
            "Press ESC to quit."
        });
    }
}

void OverworldScene::buildPalletTown() {
    // Build the map using our tile enum values directly
    std::vector<TileID> tiles = {
        // Row 0
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        // Row 1
        TileID::Tree,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 2 - buildings
        TileID::Tree,TileID::Grass,TileID::Wall,TileID::Wall,TileID::Wall,
        TileID::Door,TileID::Wall,TileID::Wall,TileID::Grass,TileID::Grass,
        TileID::Wall,TileID::Wall,TileID::Wall,TileID::Door,TileID::Wall,
        TileID::Wall,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 3
        TileID::Tree,TileID::Grass,TileID::Wall,TileID::FloorIn,TileID::FloorIn,
        TileID::FloorIn,TileID::FloorIn,TileID::Wall,TileID::Grass,TileID::Grass,
        TileID::Wall,TileID::FloorIn,TileID::FloorIn,TileID::FloorIn,TileID::FloorIn,
        TileID::Wall,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 4
        TileID::Tree,TileID::Grass,TileID::Wall,TileID::FloorIn,TileID::FloorIn,
        TileID::FloorIn,TileID::FloorIn,TileID::Wall,TileID::Grass,TileID::Grass,
        TileID::Wall,TileID::FloorIn,TileID::FloorIn,TileID::FloorIn,TileID::FloorIn,
        TileID::Wall,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 5
        TileID::Tree,TileID::Grass,TileID::Wall,TileID::Wall,TileID::Wall,
        TileID::Wall,TileID::Wall,TileID::Wall,TileID::Grass,TileID::Grass,
        TileID::Wall,TileID::Wall,TileID::Wall,TileID::Wall,TileID::Wall,
        TileID::Wall,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 6
        TileID::Tree,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Path,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Path,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Flower,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 7
        TileID::Tree,TileID::Grass,TileID::Flower,TileID::Grass,TileID::Grass,
        TileID::Path,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Path,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Flower,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 8 - horizontal road
        TileID::Tree,TileID::Path,TileID::Path,TileID::Path,TileID::Path,
        TileID::Path,TileID::Path,TileID::Path,TileID::Path,TileID::Path,
        TileID::Path,TileID::Path,TileID::Path,TileID::Path,TileID::Path,
        TileID::Path,TileID::Path,TileID::Path,TileID::Path,TileID::Tree,
        // Row 9
        TileID::Tree,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Path,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Path,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 10
        TileID::Tree,TileID::Grass,TileID::Sign,TileID::Grass,TileID::Grass,
        TileID::Path,TileID::Grass,TileID::Water,TileID::Water,TileID::Water,
        TileID::Water,TileID::Grass,TileID::Path,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 11
        TileID::Tree,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Path,TileID::Grass,TileID::Water,TileID::Water,TileID::Water,
        TileID::Water,TileID::Grass,TileID::Path,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 12
        TileID::Tree,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Path,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Path,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 13 - tall grass
        TileID::Tree,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,
        TileID::Path,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,
        TileID::TallGrass,TileID::TallGrass,TileID::Path,TileID::TallGrass,TileID::TallGrass,
        TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::Tree,
        // Row 14
        TileID::Tree,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,
        TileID::Path,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,
        TileID::TallGrass,TileID::TallGrass,TileID::Path,TileID::TallGrass,TileID::TallGrass,
        TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::Tree,
        // Row 15
        TileID::Tree,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,
        TileID::Path,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,
        TileID::TallGrass,TileID::TallGrass,TileID::Path,TileID::TallGrass,TileID::TallGrass,
        TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::TallGrass,TileID::Tree,
        // Row 16
        TileID::Tree,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Path,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Path,TileID::Grass,TileID::Grass,
        TileID::Grass,TileID::Grass,TileID::Grass,TileID::Grass,TileID::Tree,
        // Row 17
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
    };
    m_map.create(20, 18, tiles);
}

void OverworldScene::updateCamera() {
    // Center camera on player, clamped to map bounds
    auto sz    = m_game.getWindow().getSize();
    float halfW = sz.x / 2.f;
    float halfH = sz.y / 2.f;
    float maxX  = m_map.width  * TILE_SIZE - static_cast<float>(sz.x);
    float maxY  = m_map.height * TILE_SIZE - static_cast<float>(sz.y);

    sf::Vector2f pp = m_player.getPixelPos();
    float cx = pp.x + TILE_SIZE / 2.f - halfW;
    float cy = pp.y + TILE_SIZE / 2.f - halfH;
    m_cameraPos.x = std::max(0.f, std::min(cx, maxX));
    m_cameraPos.y = std::max(0.f, std::min(cy, maxY));
}

void OverworldScene::update(float dt) {
    auto& input = m_game.getInput();

    // Dialog takes priority
    if (m_dialog && m_dialog->isOpen()) {
        m_dialog->update(dt);
        if (input.isKeyPressed(sf::Keyboard::Z) ||
            input.isKeyPressed(sf::Keyboard::Return))
            m_dialog->advance();
        return;
    }

    m_player.update(dt, m_map);
    updateCamera();

    // Check wild encounter
    if (m_player.justEnteredEncounterZone())
        triggerEncounter();

    // Interact
    if (input.isKeyPressed(sf::Keyboard::Z) ||
        input.isKeyPressed(sf::Keyboard::Return))
        tryInteract();
}

void OverworldScene::tryInteract() {
    // Find tile in front of player
    auto pos = m_player.getTilePos();
    switch (m_player.getDirection()) {
    case Direction::Up:    pos.y--; break;
    case Direction::Down:  pos.y++; break;
    case Direction::Left:  pos.x--; break;
    case Direction::Right: pos.x++; break;
    }
    TileID t = m_map.get(pos.x, pos.y);
    if (t == TileID::Sign && m_dialog) {
        m_dialog->show("PALLET TOWN - Shades of your journey await!");
    }
}

void OverworldScene::triggerEncounter() {
    // Pick a random wild pokemon appropriate for the area
    static const Species pool[] = {
        Species::Rattata, Species::Pidgey, Species::Caterpie
    };
    Species wild = pool[std::rand() % 3];
    uint8_t lvl  = 3 + std::rand() % 4;

    m_wildPokemon.init(wild, lvl,
        {MoveID::Tackle, MoveID::Growl, MoveID::None, MoveID::None});

    m_game.getSceneManager().switchTo("battle");
}

void OverworldScene::render() {
    auto& win   = m_game.getWindow();
    auto& rw    = win.getRenderWindow();
    auto  sz    = win.getSize();

    // Use a view offset by camera position
    sf::View worldView(sf::FloatRect(m_cameraPos.x, m_cameraPos.y,
                                     static_cast<float>(sz.x),
                                     static_cast<float>(sz.y)));
    rw.setView(worldView);

    // Draw map (relative to camera)
    sf::Vector2i camTile(
        static_cast<int>(m_cameraPos.x / TILE_SIZE),
        static_cast<int>(m_cameraPos.y / TILE_SIZE)
    );
    // Adjust draw offset so tiles align with camera sub-tile offset
    sf::View adjustedView = worldView;
    rw.setView(adjustedView);

    // Re-draw using RenderTexture approach: just draw tiles with world coords
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    auto sz2 = win.getSize();
    int tilesX = sz2.x / TILE_SIZE + 2;
    int tilesY = sz2.y / TILE_SIZE + 2;
    for (int dy = -1; dy <= tilesY; ++dy) {
        for (int dx = -1; dx <= tilesX; ++dx) {
            int tx = camTile.x + dx;
            int ty = camTile.y + dy;
            TileID id = m_map.get(tx, ty);
            tile.setFillColor(tileColor(id));
            tile.setPosition(
                static_cast<float>(tx * TILE_SIZE),
                static_cast<float>(ty * TILE_SIZE)
            );
            rw.draw(tile);
        }
    }

    m_player.draw(rw);

    // HUD in screen space
    rw.setView(rw.getDefaultView());
    if (m_dialog) m_dialog->draw(rw, sz);

    // Mini-overlay: location name
    if (m_fontLoaded) {
        sf::Text loc;
        loc.setFont(m_font);
        loc.setCharacterSize(12);
        loc.setFillColor(sf::Color::White);
        loc.setOutlineColor(sf::Color::Black);
        loc.setOutlineThickness(1.f);
        loc.setString("PALLET TOWN");
        loc.setPosition(6.f, 6.f);
        rw.draw(loc);

        sf::Text fps;
        fps.setFont(m_font);
        fps.setCharacterSize(10);
        fps.setFillColor(sf::Color::Yellow);
        fps.setString("FPS: " + std::to_string(m_game.getTime().getFPS()));
        fps.setPosition(static_cast<float>(sz.x) - 60.f, 6.f);
        rw.draw(fps);
    }
}

} // namespace pokemon
