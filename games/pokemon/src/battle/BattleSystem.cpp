#include "battle/BattleSystem.hpp"
#include <cmath>
#include <cstdlib>
#include <sstream>

namespace pokemon {

BattleSystem::BattleSystem(const sf::Font& font) : m_font(font) {}

void BattleSystem::start(PokemonInstance* player, PokemonInstance* enemy) {
    m_player     = player;
    m_enemy      = enemy;
    m_playerWon  = false;
    m_state      = BattleState::Intro;
    m_messages.clear();
    m_msgIndex   = 0;
    m_stateTimer = 0.f;
    m_selectedMove = 0;
    m_enemyHPDisplay  = static_cast<float>(m_enemy->currentHP);
    m_playerHPDisplay = static_cast<float>(m_player->currentHP);

    pushMessage("Wild " + m_enemy->getName() + " appeared!");
    pushMessage("Go! " + m_player->getName() + "!");
    // After intro messages, move to PlayerTurn
}

void BattleSystem::pushMessage(const std::string& msg) {
    m_messages.push_back(msg);
}

void BattleSystem::nextMessage() {
    ++m_msgIndex;
    if (m_msgIndex >= m_messages.size()) {
        // Determine next state after messages
        if (m_state == BattleState::EnemyFainted) {
            m_playerWon = true;
            m_state     = BattleState::Victory;
        } else if (m_state == BattleState::PlayerFainted) {
            m_state = BattleState::Defeat;
        } else if (m_state == BattleState::Flee) {
            m_state = BattleState::Finished;
        } else if (m_state == BattleState::Victory || m_state == BattleState::Defeat) {
            m_state = BattleState::Finished;
        } else {
            m_state = BattleState::PlayerTurn;
        }
    }
}

void BattleSystem::handleInput(int moveIndex) {
    if (m_state != BattleState::PlayerTurn) {
        // Advance message on any key
        if (m_state == BattleState::ShowMessage ||
            m_state == BattleState::Intro       ||
            m_state == BattleState::Victory     ||
            m_state == BattleState::Defeat) {
            nextMessage();
        }
        return;
    }

    if (moveIndex == 4) { // Run
        m_messages.clear(); m_msgIndex = 0;
        pushMessage("Got away safely!");
        m_state = BattleState::Flee;
        return;
    }

    if (moveIndex < 0 || moveIndex >= PokemonInstance::MAX_MOVES) return;
    MoveID mv = m_player->moves[moveIndex];
    if (mv == MoveID::None || m_player->currentPP[moveIndex] == 0) return;

    m_player->currentPP[moveIndex]--;
    m_messages.clear(); m_msgIndex = 0;

    // Player attacks first (simplified: always player goes first)
    executeMove(*m_player, *m_enemy, mv, true);
    if (m_enemy->isFainted()) {
        checkFainted();
        return;
    }

    // Enemy picks a random valid move
    int enemyMove = 0;
    for (int i = 0; i < PokemonInstance::MAX_MOVES; ++i)
        if (m_enemy->moves[i] != MoveID::None && m_enemy->currentPP[i] > 0) { enemyMove=i; break; }
    MoveID emv = m_enemy->moves[enemyMove];
    m_enemy->currentPP[enemyMove]--;
    executeMove(*m_enemy, *m_player, emv, false);
    if (m_player->isFainted()) {
        checkFainted();
        return;
    }

    m_state = BattleState::ShowMessage;
}

int BattleSystem::calcDamage(const PokemonInstance& atk, const PokemonInstance& def, MoveID moveId) {
    const MoveData& mv = getMoveData(moveId);
    if (mv.power == 0) return 0;

    // Gen 1 damage formula simplified
    float atkStat  = mv.isSpecial ? static_cast<float>(atk.special) : static_cast<float>(atk.attack);
    float defStat  = mv.isSpecial ? static_cast<float>(def.special) : static_cast<float>(def.defense);
    float damage   = (2.f * atk.level / 5.f + 2.f) * mv.power * atkStat / defStat / 50.f + 2.f;

    // STAB
    const SpeciesData& atkSpecies = getSpeciesData(atk.species);
    if (mv.type == atkSpecies.type1 || mv.type == atkSpecies.type2)
        damage *= 1.5f;

    // Type effectiveness
    const SpeciesData& defSpecies = getSpeciesData(def.species);
    damage *= typeChart(mv.type, defSpecies.type1);
    damage *= typeChart(mv.type, defSpecies.type2);

    // Random factor (85-100%)
    float r = 0.85f + (std::rand() % 16) / 100.f;
    damage *= r;

    return std::max(1, static_cast<int>(damage));
}

void BattleSystem::executeMove(PokemonInstance& attacker, PokemonInstance& defender,
                                MoveID moveId, bool isPlayer) {
    const MoveData& mv = getMoveData(moveId);
    pushMessage(attacker.getName() + " used " + mv.name + "!");

    if (mv.power == 0) {
        pushMessage("(Status move)");
        return;
    }

    // Accuracy check
    int acc = mv.accuracy;
    if (std::rand() % 100 >= acc) {
        pushMessage("It missed!");
        return;
    }

    int dmg = calcDamage(attacker, defender, moveId);
    defender.currentHP = std::max(0, defender.currentHP - dmg);

    // Effectiveness message
    const SpeciesData& defSp = getSpeciesData(defender.species);
    float eff = typeChart(mv.type, defSp.type1) * typeChart(mv.type, defSp.type2);
    if (eff > 1.f)       pushMessage("It's super effective!");
    else if (eff < 0.5f) pushMessage("It's not very effective...");
    else if (eff == 0.f) pushMessage("It doesn't affect " + defender.getName() + "...");
}

void BattleSystem::checkFainted() {
    m_messages.clear(); m_msgIndex = 0;
    if (m_enemy->isFainted()) {
        pushMessage(m_enemy->getName() + " fainted!");
        m_state = BattleState::EnemyFainted;
    } else {
        pushMessage(m_player->getName() + " fainted!");
        m_state = BattleState::PlayerFainted;
    }
}

void BattleSystem::update(float dt) {
    // Animate HP bars
    float target = m_enemy  ? static_cast<float>(m_enemy->currentHP)  : 0.f;
    m_enemyHPDisplay  += (target - m_enemyHPDisplay)  * std::min(1.f, dt * 5.f);

    float ptarget = m_player ? static_cast<float>(m_player->currentHP) : 0.f;
    m_playerHPDisplay += (ptarget - m_playerHPDisplay) * std::min(1.f, dt * 5.f);
}

// ---- Drawing ---------------------------------------------------------------

void BattleSystem::drawHPBar(sf::RenderTarget& t, sf::Vector2f pos, float fraction, float w) const {
    fraction = std::max(0.f, std::min(1.f, fraction));
    sf::RectangleShape bg(sf::Vector2f(w, 8.f));
    bg.setPosition(pos);
    bg.setFillColor(sf::Color(80, 80, 80));
    t.draw(bg);

    sf::Color barColor = fraction > 0.5f ? sf::Color(80, 200, 80)
                       : fraction > 0.2f ? sf::Color(240, 200, 40)
                                         : sf::Color(220, 60, 60);
    sf::RectangleShape bar(sf::Vector2f(w * fraction, 8.f));
    bar.setPosition(pos);
    bar.setFillColor(barColor);
    t.draw(bar);
}

void BattleSystem::drawPokemon(sf::RenderTarget& t, const PokemonInstance& p,
                                sf::Vector2f pos, bool flip) const {
    const SpeciesData& sp = getSpeciesData(p.species);
    sf::Color c((sp.color >> 24) & 0xFF, (sp.color >> 16) & 0xFF,
                (sp.color >> 8)  & 0xFF, sp.color & 0xFF);

    float sz = flip ? 48.f : 64.f;
    sf::RectangleShape body(sf::Vector2f(sz, sz));
    body.setOrigin(sz/2.f, sz/2.f);
    body.setPosition(pos);
    body.setFillColor(c);
    body.setOutlineColor(sf::Color::Black);
    body.setOutlineThickness(2.f);
    t.draw(body);

    // Label name above
    sf::Text lbl;
    lbl.setFont(m_font);
    lbl.setCharacterSize(10);
    lbl.setFillColor(sf::Color::Black);
    lbl.setString(p.getName() + " Lv" + std::to_string(p.level));
    lbl.setPosition(pos.x - sz/2.f, pos.y - sz/2.f - 14.f);
    t.draw(lbl);
}

void BattleSystem::drawBattleBox(sf::RenderTarget& t, sf::Vector2u sz) const {
    float W = static_cast<float>(sz.x);
    float H = static_cast<float>(sz.y);

    // Background
    sf::RectangleShape bg(sf::Vector2f(W, H));
    bg.setFillColor(sf::Color(200, 220, 200));
    t.draw(bg);

    // Ground strips
    sf::RectangleShape ground(sf::Vector2f(W, 20.f));
    ground.setPosition(0.f, H * 0.55f);
    ground.setFillColor(sf::Color(160, 120, 80));
    t.draw(ground);
    ground.setSize(sf::Vector2f(W, 20.f));
    ground.setPosition(0.f, H * 0.35f);
    ground.setFillColor(sf::Color(160, 120, 80));
    t.draw(ground);

    if (!m_enemy || !m_player) return;

    // Enemy pokemon (back-left, smaller because front)
    drawPokemon(t, *m_enemy,  {W * 0.25f, H * 0.28f}, false);
    drawPokemon(t, *m_player, {W * 0.75f, H * 0.48f}, true);

    // Enemy HP bar
    float ehpFrac = m_enemy->maxHP > 0
        ? m_enemyHPDisplay / static_cast<float>(m_enemy->maxHP) : 0.f;
    drawHPBar(t, {W * 0.05f, H * 0.12f}, ehpFrac, 120.f);

    sf::Text ehp;
    ehp.setFont(m_font);
    ehp.setCharacterSize(10);
    ehp.setFillColor(sf::Color::Black);
    ehp.setString("HP: " + std::to_string(m_enemy->currentHP) + "/" + std::to_string(m_enemy->maxHP));
    ehp.setPosition(W * 0.05f, H * 0.12f + 10.f);
    t.draw(ehp);

    // Player HP bar
    float phpFrac = m_player->maxHP > 0
        ? m_playerHPDisplay / static_cast<float>(m_player->maxHP) : 0.f;
    drawHPBar(t, {W * 0.55f, H * 0.62f}, phpFrac, 120.f);

    sf::Text php;
    php.setFont(m_font);
    php.setCharacterSize(10);
    php.setFillColor(sf::Color::Black);
    php.setString("HP: " + std::to_string(m_player->currentHP) + "/" + std::to_string(m_player->maxHP));
    php.setPosition(W * 0.55f, H * 0.62f + 10.f);
    t.draw(php);

    // Message box
    float msgY = H * 0.72f;
    sf::RectangleShape msgBg(sf::Vector2f(W, H - msgY));
    msgBg.setPosition(0.f, msgY);
    msgBg.setFillColor(sf::Color(240, 240, 240));
    msgBg.setOutlineColor(sf::Color::Black);
    msgBg.setOutlineThickness(2.f);
    t.draw(msgBg);

    if (m_state == BattleState::PlayerTurn) {
        drawMoveMenu(t, sz);
    } else {
        // Show current message
        if (m_msgIndex < m_messages.size()) {
            sf::Text msg;
            msg.setFont(m_font);
            msg.setCharacterSize(12);
            msg.setFillColor(sf::Color::Black);
            msg.setString(m_messages[m_msgIndex]);
            msg.setPosition(10.f, msgY + 10.f);
            t.draw(msg);
        }
    }
}

void BattleSystem::drawMoveMenu(sf::RenderTarget& t, sf::Vector2u sz) const {
    if (!m_player) return;
    float W = static_cast<float>(sz.x);
    float H = static_cast<float>(sz.y);
    float msgY = H * 0.72f;

    sf::Text prompt;
    prompt.setFont(m_font);
    prompt.setCharacterSize(12);
    prompt.setFillColor(sf::Color::Black);
    prompt.setString("What will " + m_player->getName() + " do?");
    prompt.setPosition(10.f, msgY + 8.f);
    t.draw(prompt);

    float colW = W / 2.f;
    for (int i = 0; i < PokemonInstance::MAX_MOVES; ++i) {
        MoveID mv = m_player->moves[i];
        if (mv == MoveID::None) continue;
        const MoveData& md = getMoveData(mv);

        float x = (i % 2) * colW + 10.f;
        float y = msgY + 30.f + (i / 2) * 20.f;

        // Cursor
        if (i == m_selectedMove) {
            sf::RectangleShape sel(sf::Vector2f(colW - 20.f, 18.f));
            sel.setPosition(x - 2.f, y - 1.f);
            sel.setFillColor(sf::Color(180, 200, 255));
            t.draw(sel);
        }

        sf::Text moveText;
        moveText.setFont(m_font);
        moveText.setCharacterSize(11);
        moveText.setFillColor(sf::Color::Black);
        moveText.setString(md.name + " PP:" + std::to_string(m_player->currentPP[i]));
        moveText.setPosition(x, y);
        t.draw(moveText);
    }

    // Run option
    sf::Text run;
    run.setFont(m_font);
    run.setCharacterSize(11);
    run.setFillColor(sf::Color(180, 60, 60));
    run.setString("[R] Run");
    run.setPosition(W - 80.f, msgY + 8.f);
    t.draw(run);
}

void BattleSystem::draw(sf::RenderTarget& target, sf::Vector2u screenSize) const {
    drawBattleBox(target, screenSize);
}

} // namespace pokemon
