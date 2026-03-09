#pragma once
#include "scene/Scene.hpp"
#include "tilemap/TileMap.hpp"
#include "player/Player.hpp"
#include "dialog/DialogSystem.hpp"
#include <SFML/Graphics.hpp>

namespace pokemon {

class OverworldScene : public engine::Scene {
public:
    // party/partySize and wildPokemon are owned by PokemonGame (outlive the scene)
    OverworldScene(engine::Game& game,
                   PokemonInstance* party, int partySize,
                   PokemonInstance& wildPokemon);

    void onEnter() override;
    void update(float dt) override;
    void render() override;

private:
    void buildPalletTown();
    void tryInteract();
    void triggerEncounter();

    TileMap      m_map;
    Player       m_player;
    sf::Font     m_font;
    bool         m_fontLoaded = false;

    std::unique_ptr<DialogSystem> m_dialog;

    sf::Vector2f m_cameraPos;
    void updateCamera();

    // Shared state references (owned by PokemonGame)
    PokemonInstance* m_party;
    int              m_partySize;
    PokemonInstance& m_wildPokemon;
};

} // namespace pokemon
