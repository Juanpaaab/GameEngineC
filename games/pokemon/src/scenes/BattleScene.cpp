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
    // Try common font paths in order: Windows, Linux (multiple distros), macOS
    auto tryLoad = [&](const char* path) { return m_font.openFromFile(path); };
    m_fontLoaded =
        tryLoad("C:/Windows/Fonts/arial.ttf")   ||
        tryLoad("C:/Windows/Fonts/calibri.ttf") ||
        tryLoad("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf") ||
        tryLoad("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
        tryLoad("/usr/share/fonts/TTF/DejaVuSans.ttf")             ||
        tryLoad("/Library/Fonts/Arial.ttf");

    if (!m_fontLoaded)
        std::fprintf(stderr, "[BattleScene] WARNING: could not load any system font.\n");

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
        if (input.isKeyPressed(sf::Keyboard::Key::Up))    m_selectedMove = (m_selectedMove - 2 + 4) % 4;
        if (input.isKeyPressed(sf::Keyboard::Key::Down))  m_selectedMove = (m_selectedMove + 2) % 4;
        if (input.isKeyPressed(sf::Keyboard::Key::Left))  m_selectedMove = (m_selectedMove - 1 + 4) % 4;
        if (input.isKeyPressed(sf::Keyboard::Key::Right)) m_selectedMove = (m_selectedMove + 1) % 4;

        if (input.isKeyPressed(sf::Keyboard::Key::Z) ||
            input.isKeyPressed(sf::Keyboard::Key::Enter))
            m_battle->handleInput(m_selectedMove);

        if (input.isKeyPressed(sf::Keyboard::Key::R) ||
            input.isKeyPressed(sf::Keyboard::Key::Escape))
            m_battle->handleInput(4); // run
    } else {
        // Advance message on Z/Enter/Space
        if (input.isKeyPressed(sf::Keyboard::Key::Z)     ||
            input.isKeyPressed(sf::Keyboard::Key::Enter) ||
            input.isKeyPressed(sf::Keyboard::Key::Space))
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
