#pragma once
#include "scene/Scene.hpp"
#include "battle/BattleSystem.hpp"
#include "data/PokemonData.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace pokemon {

class BattleScene : public engine::Scene {
public:
    explicit BattleScene(engine::Game& game,
                         PokemonInstance* playerPokemon,
                         PokemonInstance* wildPokemon);

    void onEnter()  override;
    void update(float dt) override;
    void render() override;

private:
    sf::Font    m_font;
    bool        m_fontLoaded = false;

    PokemonInstance* m_playerPokemon;
    PokemonInstance* m_wildPokemon;

    std::unique_ptr<BattleSystem> m_battle;

    int m_selectedMove = 0;
    static const int MOVE_COUNT = PokemonInstance::MAX_MOVES;
};

} // namespace pokemon
