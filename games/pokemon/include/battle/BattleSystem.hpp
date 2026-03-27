#pragma once
#include "data/PokemonData.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

namespace pokemon {

enum class BattleState {
    Intro,          // "Wild X appeared!"
    PlayerTurn,     // waiting for move selection
    EnemyTurn,      // enemy picks move
    ShowMessage,    // show damage/effect message
    LevelUp,        // level up animation
    PlayerFainted,
    EnemyFainted,
    Victory,
    Defeat,
    Flee,
    Finished
};

struct BattleAction {
    enum class Type { Move, Bag, Pokemon, Run } type;
    int index = 0; // move index (0-3) or bag item index
};

class BattleSystem {
public:
    BattleSystem(const sf::Font& font);

    // Start a battle. player/enemy are non-owning pointers.
    void start(PokemonInstance* player, PokemonInstance* enemy);
    bool isFinished() const { return m_state == BattleState::Finished; }
    bool playerWon()  const { return m_playerWon; }

    void handleInput(int moveIndex);  // 0-3 for moves, 4 for run
    void setSelectedMove(int i) { m_selectedMove = i; }
    void update(float dt);
    void draw(sf::RenderTarget& target, sf::Vector2u screenSize) const;

    BattleState getState() const { return m_state; }

private:
    void executeMove(PokemonInstance& attacker, PokemonInstance& defender,
                     MoveID moveId, bool isPlayer);
    void pushMessage(const std::string& msg);
    void nextMessage();
    void checkFainted();
    void applyEXP();
    void applyEndOfTurnStatus(PokemonInstance& p);
    bool canMove(PokemonInstance& p, bool isPlayer);
    int  calcDamage(const PokemonInstance& atk, const PokemonInstance& def, MoveID move);

    const sf::Font&     m_font;
    PokemonInstance*    m_player  = nullptr;
    PokemonInstance*    m_enemy   = nullptr;
    BattleState         m_state   = BattleState::Finished;
    bool                m_playerWon = false;

    std::vector<std::string> m_messages;
    std::size_t              m_msgIndex = 0;

    float   m_stateTimer  = 0.f;
    int     m_selectedMove = 0;

    // HP bar animation
    float   m_enemyHPDisplay  = 0.f;
    float   m_playerHPDisplay = 0.f;

    // Stat stages per battle (-6 to +6)
    int m_playerAtkStage = 0, m_playerDefStage = 0;
    int m_enemyAtkStage  = 0, m_enemyDefStage  = 0;

    // Sprites: front (enemy) and back (player)
    sf::Texture m_playerTex;
    sf::Texture m_enemyTex;
    bool        m_playerTexLoaded = false;
    bool        m_enemyTexLoaded  = false;
    void loadSprites();

    void drawHPBar(sf::RenderTarget& t, sf::Vector2f pos, float fraction, float w) const;
    // tex: nullptr falls back to colored placeholder rectangle
    void drawPokemon(sf::RenderTarget& t, const PokemonInstance& p,
                     sf::Vector2f pos, bool smallSprite, const sf::Texture* tex) const;
    void drawMoveMenu(sf::RenderTarget& t, sf::Vector2u sz) const;
    void drawBattleBox(sf::RenderTarget& t, sf::Vector2u sz) const;
};

} // namespace pokemon
