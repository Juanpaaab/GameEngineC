#include "core/Game.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/BattleScene.hpp"
#include "data/PokemonData.hpp"
#include <memory>
#include <array>

// Shared game state that outlives individual scenes
struct PokemonState {
    // Player's party (owned here, scenes get raw pointers)
    std::array<pokemon::PokemonInstance, 6> party;
    int partySize = 0;

    pokemon::PokemonInstance wildPokemon;

    void addToParty(pokemon::Species s, uint8_t level,
                    const std::array<pokemon::MoveID, pokemon::PokemonInstance::MAX_MOVES>& moves) {
        if (partySize >= 6) return;
        party[partySize++].init(s, level, moves);
    }
};

class PokemonGame : public engine::Game {
public:
    PokemonGame()
        : engine::Game({160*3, 144*3, "Pokemon Red - C++ Clone", 60, false})
    {}

protected:
    void onInit() override {
        // Give the player a starter
        m_state.addToParty(pokemon::Species::Charmander, 5,
            {pokemon::MoveID::Scratch, pokemon::MoveID::Growl,
             pokemon::MoveID::Ember,   pokemon::MoveID::None});

        getSceneManager().registerScene("overworld", [this](engine::Game& g) {
            return std::make_unique<pokemon::OverworldScene>(g, m_state.party.data(),
                                                              m_state.partySize,
                                                              m_state.wildPokemon);
        });

        getSceneManager().registerScene("battle", [this](engine::Game& g) {
            return std::make_unique<pokemon::BattleScene>(g,
                &m_state.party[0],
                &m_state.wildPokemon);
        });

        getSceneManager().switchTo("overworld");
    }

    void onUpdate(float /*dt*/) override {
        if (getInput().isKeyPressed(sf::Keyboard::Escape))
            quit();
    }

private:
    PokemonState m_state;
};

int main() {
    PokemonGame game;
    game.run();
    return 0;
}
