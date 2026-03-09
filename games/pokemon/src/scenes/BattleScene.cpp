#include "scenes/BattleScene.hpp"
#include "core/Game.hpp"

namespace pokemon {

BattleScene::BattleScene(engine::Game& game,
                         PokemonInstance* playerPokemon,
                         PokemonInstance* wildPokemon)
    : engine::Scene(game)
    , m_playerPokemon(playerPokemon)
    , m_wildPokemon(wildPokemon)
{}

void BattleScene::onEnter() {
    m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    if (!m_fontLoaded)
        m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");

    if (m_fontLoaded) {
        m_battle = std::make_unique<BattleSystem>(m_font);
        m_battle->start(m_playerPokemon, m_wildPokemon);
    }
    m_selectedMove = 0;
}

void BattleScene::update(float dt) {
    if (!m_battle) return;

    m_battle->update(dt);

    if (m_battle->isFinished()) {
        m_game.getSceneManager().switchTo("overworld");
        return;
    }

    auto& input = m_game.getInput();
    auto state  = m_battle->getState();

    if (state == BattleState::PlayerTurn) {
        // Navigate move selection
        if (input.isKeyPressed(sf::Keyboard::Up))    m_selectedMove = (m_selectedMove - 2 + 4) % 4;
        if (input.isKeyPressed(sf::Keyboard::Down))  m_selectedMove = (m_selectedMove + 2) % 4;
        if (input.isKeyPressed(sf::Keyboard::Left))  m_selectedMove = (m_selectedMove - 1 + 4) % 4;
        if (input.isKeyPressed(sf::Keyboard::Right)) m_selectedMove = (m_selectedMove + 1) % 4;

        if (input.isKeyPressed(sf::Keyboard::Z) ||
            input.isKeyPressed(sf::Keyboard::Return))
            m_battle->handleInput(m_selectedMove);

        if (input.isKeyPressed(sf::Keyboard::R) ||
            input.isKeyPressed(sf::Keyboard::Escape))
            m_battle->handleInput(4); // run
    } else {
        // Advance message on Z/Enter/Space
        if (input.isKeyPressed(sf::Keyboard::Z)     ||
            input.isKeyPressed(sf::Keyboard::Return) ||
            input.isKeyPressed(sf::Keyboard::Space))
            m_battle->handleInput(0);
    }
}

void BattleScene::render() {
    if (!m_battle) return;
    auto& rw = m_game.getWindow().getRenderWindow();
    rw.setView(rw.getDefaultView());
    auto sz = m_game.getWindow().getSize();
    m_battle->draw(rw, sz);
}

} // namespace pokemon
