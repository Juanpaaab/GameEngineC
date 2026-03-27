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
    // Try common font paths in order: Windows, Linux (multiple distros), macOS
    auto tryLoad = [&](const char* path) { return m_font.openFromFile(path); };
    m_fontLoaded =
        tryLoad("C:/Windows/Fonts/arial.ttf")   ||
        tryLoad("C:/Windows/Fonts/calibri.ttf") ||
        tryLoad("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf") ||
        tryLoad("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
        tryLoad("/usr/share/fonts/TTF/DejaVuSans.ttf")             ||
        tryLoad("/Library/Fonts/Arial.ttf");

    if (m_fontLoaded)
        m_dialog = std::make_unique<DialogSystem>(m_font);

    buildPalletTown();

    m_player.setPosition(5, 9);
    // Cargar spritesheet del jugador (Verde Hoja: 4 dir x 3 frames, TILE_SIZE x TILE_SIZE c/u)
    m_player.loadTexture("assets/player/player.png");

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
        // Row 0 — north border; Door at x=5 and x=12 lead to Route 1
        TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Door,TileID::Tree,TileID::Tree,TileID::Tree,TileID::Tree,
        TileID::Tree,TileID::Tree,TileID::Door,TileID::Tree,TileID::Tree,
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

    // Populate NPCs for this map
    m_npcs = {
        { {7, 9},  "Hi there! Have you visited Prof. Oak's Lab?\nHe gives POKEMON to new trainers!" },
        { {14, 7}, "PALLET TOWN - Tiny and quiet.\nA great place to begin your journey." }
    };
}

void OverworldScene::buildRoute1() {
    // Route 1: 20 wide x 20 tall, mostly tall grass with center paths.
    // Door tiles at row 19 (x=5,12) lead back to Pallet Town.
    using T = TileID;
    std::vector<TileID> tiles;
    tiles.reserve(20 * 20);
    for (int row = 0; row < 20; ++row) {
        for (int col = 0; col < 20; ++col) {
            if (col == 0 || col == 19) { tiles.push_back(T::Tree); continue; }
            if (row == 0)              { tiles.push_back(T::Tree); continue; }
            if (row == 19) {
                // South exit matching Pallet Town north exits
                tiles.push_back((col == 5 || col == 12) ? T::Door : T::Tree);
                continue;
            }
            if (col == 5 || col == 12) { tiles.push_back(T::Path);      continue; }
            // Alternate tall/normal grass; leave a small clearing in the middle
            if (row >= 8 && row <= 10 && col >= 6 && col <= 11) {
                tiles.push_back(T::Grass); // NPC standing area
            } else if (row % 3 == 0 && col % 4 == 0) {
                tiles.push_back(T::Grass);
            } else {
                tiles.push_back(T::TallGrass);
            }
        }
    }
    m_map.create(20, 20, tiles);

    // Populate NPCs for Route 1
    m_npcs = {
        { {9, 9},  "ROUTE 1 - On the way to VIRIDIAN CITY!\nWatch out for wild POKEMON in the tall grass." },
        { {7, 5},  "The tall grass here hides many wild POKEMON.\nBe sure to heal at a POKEMON CENTER!" }
    };
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
        if (input.isKeyPressed(sf::Keyboard::Key::Z) ||
            input.isKeyPressed(sf::Keyboard::Key::Enter))
            m_dialog->advance();
        return;
    }

    m_player.update(dt, m_map);
    updateCamera();

    // Check map transitions via Door tiles
    auto playerTile = m_player.getTilePos();
    TileID tileUnder = m_map.get(playerTile.x, playerTile.y);
    if (tileUnder == TileID::Door) {
        if (m_currentMapId == 0 && playerTile.y == 0) {
            // North exit — enter Route 1 from the bottom
            m_currentMapId = 1;
            buildRoute1();
            m_player.setPosition(playerTile.x, 18);
            if (m_dialog) m_dialog->show("ROUTE 1");
        } else if (m_currentMapId == 1 && playerTile.y == 19) {
            // South exit — back to Pallet Town
            m_currentMapId = 0;
            buildPalletTown();
            m_player.setPosition(playerTile.x, 1);
            if (m_dialog) m_dialog->show("PALLET TOWN");
        }
        updateCamera();
    }

    // Check wild encounter
    if (m_player.justEnteredEncounterZone())
        triggerEncounter();

    // Interact
    if (input.isKeyPressed(sf::Keyboard::Key::Z) ||
        input.isKeyPressed(sf::Keyboard::Key::Enter))
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

    // Check NPCs first
    for (const auto& npc : m_npcs) {
        if (npc.tile.x == pos.x && npc.tile.y == pos.y && m_dialog) {
            m_dialog->show(npc.dialog);
            return;
        }
    }

    TileID t = m_map.get(pos.x, pos.y);
    if (t == TileID::Sign && m_dialog) {
        m_dialog->show("PALLET TOWN - Shades of your journey await!");
    }
}

void OverworldScene::triggerEncounter() {
    // Pick a random wild pokemon appropriate for the current map
    uint8_t lvl = 0;
    Species wild = Species::None;
    if (m_currentMapId == 0) {
        static const Species pool[] = { Species::Rattata, Species::Pidgey, Species::Caterpie };
        wild = pool[std::rand() % 3];
        lvl  = 3 + std::rand() % 4; // levels 3-6
    } else {
        static const Species pool[] = { Species::Pidgey, Species::Rattata, Species::Weedle };
        wild = pool[std::rand() % 3];
        lvl  = 3 + std::rand() % 5; // levels 3-7
    }

    m_wildPokemon.init(wild, lvl,
        {MoveID::Tackle, MoveID::Growl, MoveID::None, MoveID::None});

    m_game.getSceneManager().switchTo("battle");
}

void OverworldScene::render() {
    auto& win   = m_game.getWindow();
    auto& rw    = win.getRenderWindow();
    auto  sz    = win.getSize();

    // Use a view offset by camera position
    sf::View worldView(sf::FloatRect({m_cameraPos.x, m_cameraPos.y},
                                     {static_cast<float>(sz.x), static_cast<float>(sz.y)}));
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
                {static_cast<float>(tx * TILE_SIZE),
                 static_cast<float>(ty * TILE_SIZE)}
            );
            rw.draw(tile);
        }
    }

    m_player.draw(rw);

    // Draw NPCs as blue rectangles
    sf::RectangleShape npcShape(sf::Vector2f(TILE_SIZE - 4.f, TILE_SIZE - 4.f));
    npcShape.setFillColor(sf::Color(80, 100, 200));
    npcShape.setOutlineColor(sf::Color::Black);
    npcShape.setOutlineThickness(1.f);
    for (const auto& npc : m_npcs) {
        npcShape.setPosition(
            {static_cast<float>(npc.tile.x * TILE_SIZE + 2),
             static_cast<float>(npc.tile.y * TILE_SIZE + 2)});
        rw.draw(npcShape);
    }

    // HUD in screen space
    rw.setView(rw.getDefaultView());
    if (m_dialog) m_dialog->draw(rw, sz);

    // Mini-overlay: location name
    if (m_fontLoaded) {
        sf::Text loc(m_font);
        loc.setCharacterSize(12);
        loc.setFillColor(sf::Color::White);
        loc.setOutlineColor(sf::Color::Black);
        loc.setOutlineThickness(1.f);
        loc.setString(m_currentMapId == 0 ? "PALLET TOWN" : "ROUTE 1");
        loc.setPosition({6.f, 6.f});
        rw.draw(loc);

        sf::Text fps(m_font);
        fps.setCharacterSize(10);
        fps.setFillColor(sf::Color::Yellow);
        fps.setString("FPS: " + std::to_string(m_game.getTime().getFPS()));
        fps.setPosition({static_cast<float>(sz.x) - 60.f, 6.f});
        rw.draw(fps);
    }
}

} // namespace pokemon
