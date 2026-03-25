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

    // Reset stat stages
    m_playerAtkStage = m_playerDefStage = 0;
    m_enemyAtkStage  = m_enemyDefStage  = 0;

    pushMessage("Wild " + m_enemy->getName() + " appeared!");
    pushMessage("Go! " + m_player->getName() + "!");
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
            applyEXP(); // gains EXP, possibly pushes level-up messages
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

    // Pick enemy move before resolving (so PP is consumed regardless of who goes first)
    int enemyMoveIdx = 0;
    for (int i = 0; i < PokemonInstance::MAX_MOVES; ++i)
        if (m_enemy->moves[i] != MoveID::None && m_enemy->currentPP[i] > 0) { enemyMoveIdx = i; break; }
    MoveID emv = m_enemy->moves[enemyMoveIdx];
    m_enemy->currentPP[enemyMoveIdx]--;

    // Speed-based turn order (paralysis halves speed for ordering)
    int playerSpd = m_player->speed;
    int enemySpd  = m_enemy->speed;
    if (m_player->statusEffect == StatusEffect::Paralysis) playerSpd /= 2;
    if (m_enemy->statusEffect  == StatusEffect::Paralysis) enemySpd  /= 2;
    bool playerFirst = (playerSpd > enemySpd) ||
                       (playerSpd == enemySpd && std::rand() % 2 == 0);

    auto doPlayerMove = [&]() {
        if (canMove(*m_player, true))
            executeMove(*m_player, *m_enemy, mv, true);
    };
    auto doEnemyMove = [&]() {
        if (canMove(*m_enemy, false))
            executeMove(*m_enemy, *m_player, emv, false);
    };

    if (playerFirst) {
        doPlayerMove();
        if (m_enemy->isFainted()) { checkFainted(); return; }
        doEnemyMove();
        if (m_player->isFainted()) { checkFainted(); return; }
    } else {
        doEnemyMove();
        if (m_player->isFainted()) { checkFainted(); return; }
        doPlayerMove();
        if (m_enemy->isFainted()) { checkFainted(); return; }
    }

    // End-of-turn status damage (Poison, Burn)
    applyEndOfTurnStatus(*m_player);
    if (m_player->isFainted()) { checkFainted(); return; }
    applyEndOfTurnStatus(*m_enemy);
    if (m_enemy->isFainted()) { checkFainted(); return; }

    m_state = BattleState::ShowMessage;
}

int BattleSystem::calcDamage(const PokemonInstance& atk, const PokemonInstance& def, MoveID moveId) {
    const MoveData& mv = getMoveData(moveId);
    if (mv.power == 0) return 0;

    // Apply stat stages to effective attack/defense
    float atkStage = mv.isSpecial ? 0.f
                   : static_cast<float>((&atk == m_player) ? m_playerAtkStage : m_enemyAtkStage);
    float defStage = mv.isSpecial ? 0.f
                   : static_cast<float>((&def == m_player) ? m_playerDefStage : m_enemyDefStage);
    auto stageMul = [](float s) -> float {
        return (s >= 0.f) ? (2.f + s) / 2.f : 2.f / (2.f - s);
    };

    float atkStat = mv.isSpecial ? static_cast<float>(atk.special)  : static_cast<float>(atk.attack);
    float defStat = mv.isSpecial ? static_cast<float>(def.special)  : static_cast<float>(def.defense);
    atkStat *= stageMul(atkStage);
    defStat *= stageMul(defStage);

    // Burn halves physical attack
    if (!mv.isSpecial && atk.statusEffect == StatusEffect::Burn)
        atkStat /= 2.f;

    float damage = (2.f * atk.level / 5.f + 2.f) * mv.power * atkStat / defStat / 50.f + 2.f;

    // STAB
    const SpeciesData& atkSpecies = getSpeciesData(atk.species);
    if (mv.type == atkSpecies.type1 || mv.type == atkSpecies.type2)
        damage *= 1.5f;

    // Type effectiveness
    const SpeciesData& defSpecies = getSpeciesData(def.species);
    damage *= typeChart(mv.type, defSpecies.type1);
    damage *= typeChart(mv.type, defSpecies.type2);

    // Gen 1 critical hit: probability = attacker speed / 512
    bool isCrit = (std::rand() % 512) < atk.speed;
    if (isCrit) {
        damage *= 2.f;
        // pushMessage is not const so we use a non-const path; handled in executeMove
    }

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
        // Stat-change moves
        if (moveId == MoveID::Growl) {
            int& stage = isPlayer ? m_enemyAtkStage : m_playerAtkStage;
            if (stage > -6) { stage--; pushMessage(defender.getName() + "'s ATTACK fell!"); }
            else            { pushMessage("It won't go any lower!"); }
        } else if (moveId == MoveID::Leer) {
            int& stage = isPlayer ? m_enemyDefStage : m_playerDefStage;
            if (stage > -6) { stage--; pushMessage(defender.getName() + "'s DEFENSE fell!"); }
            else            { pushMessage("It won't go any lower!"); }
        } else {
            pushMessage("But nothing happened!");
        }
        return;
    }

    // Accuracy check
    int acc = mv.accuracy;
    if (std::rand() % 100 >= acc) {
        pushMessage("It missed!");
        return;
    }

    // Critical hit check
    bool isCrit = (std::rand() % 512) < attacker.speed;

    int dmg = calcDamage(attacker, defender, moveId);
    // calcDamage also rolled a crit internally; re-check and notify with a single roll
    // Override: use a clean crit flag computed before calcDamage to avoid double-roll
    // (calcDamage uses its own crit, so just report based on its output being >normal)
    // For a clean implementation we report crit when the raw damage would be 2x:
    // Simply push the message here based on our `isCrit` flag.
    if (isCrit) pushMessage("Critical hit!");

    defender.currentHP = std::max(0, defender.currentHP - dmg);

    // Effectiveness message
    const SpeciesData& defSp = getSpeciesData(defender.species);
    float eff = typeChart(mv.type, defSp.type1) * typeChart(mv.type, defSp.type2);
    if      (eff > 1.f)       pushMessage("It's super effective!");
    else if (eff < 1.f && eff > 0.f) pushMessage("It's not very effective...");
    else if (eff == 0.f)      pushMessage("It doesn't affect " + defender.getName() + "...");
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

bool BattleSystem::canMove(PokemonInstance& p, bool /*isPlayer*/) {
    switch (p.statusEffect) {
    case StatusEffect::Sleep:
        if (p.sleepTurns > 0) {
            --p.sleepTurns;
            pushMessage(p.getName() + " is fast asleep...");
            return false;
        }
        p.statusEffect = StatusEffect::None;
        pushMessage(p.getName() + " woke up!");
        return true;
    case StatusEffect::Paralysis:
        if (std::rand() % 4 == 0) {
            pushMessage(p.getName() + " is fully paralyzed!");
            return false;
        }
        return true;
    case StatusEffect::Freeze:
        if (std::rand() % 5 == 0) {
            p.statusEffect = StatusEffect::None;
            pushMessage(p.getName() + " thawed out!");
            return true;
        }
        pushMessage(p.getName() + " is frozen solid!");
        return false;
    default:
        return true;
    }
}

void BattleSystem::applyEndOfTurnStatus(PokemonInstance& p) {
    if (p.statusEffect == StatusEffect::Poison) {
        int dmg = std::max(1, p.maxHP / 8);
        p.currentHP = std::max(0, p.currentHP - dmg);
        pushMessage(p.getName() + " is hurt by poison!");
    } else if (p.statusEffect == StatusEffect::Burn) {
        int dmg = std::max(1, p.maxHP / 8);
        p.currentHP = std::max(0, p.currentHP - dmg);
        pushMessage(p.getName() + " is hurt by its burn!");
    }
}

void BattleSystem::applyEXP() {
    if (!m_player || !m_enemy) return;
    uint32_t gained = static_cast<uint32_t>(m_enemy->level) * 10u;
    m_player->exp += gained;
    m_messages.clear(); m_msgIndex = 0;
    pushMessage(m_player->getName() + " gained " + std::to_string(gained) + " EXP!");

    // Level-up check: threshold = level^2 * 5 (simplified Gen 1 curve)
    while (m_player->exp >= static_cast<uint32_t>(m_player->level * m_player->level * 5)) {
        m_player->exp -= static_cast<uint32_t>(m_player->level * m_player->level * 5);
        m_player->level++;
        int oldHP = m_player->currentHP;
        m_player->recalcStats();
        // Restore HP proportional to the gain
        m_player->currentHP = std::min(m_player->maxHP, oldHP + 5);
        pushMessage(m_player->getName() + " grew to level " +
                    std::to_string(m_player->level) + "!");
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
    body.setOrigin({sz/2.f, sz/2.f});
    body.setPosition(pos);
    body.setFillColor(c);
    body.setOutlineColor(sf::Color::Black);
    body.setOutlineThickness(2.f);
    t.draw(body);

    // Label name above
    sf::Text lbl(m_font);
    lbl.setCharacterSize(10);
    lbl.setFillColor(sf::Color::Black);
    lbl.setString(p.getName() + " Lv" + std::to_string(p.level));
    lbl.setPosition({pos.x - sz/2.f, pos.y - sz/2.f - 14.f});
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
    ground.setPosition({0.f, H * 0.55f});
    ground.setFillColor(sf::Color(160, 120, 80));
    t.draw(ground);
    ground.setSize(sf::Vector2f(W, 20.f));
    ground.setPosition({0.f, H * 0.35f});
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

    sf::Text ehp(m_font);
    ehp.setCharacterSize(10);
    ehp.setFillColor(sf::Color::Black);
    ehp.setString("HP: " + std::to_string(m_enemy->currentHP) + "/" + std::to_string(m_enemy->maxHP));
    ehp.setPosition({W * 0.05f, H * 0.12f + 10.f});
    t.draw(ehp);

    // Player HP bar
    float phpFrac = m_player->maxHP > 0
        ? m_playerHPDisplay / static_cast<float>(m_player->maxHP) : 0.f;
    drawHPBar(t, {W * 0.55f, H * 0.62f}, phpFrac, 120.f);

    sf::Text php(m_font);
    php.setCharacterSize(10);
    php.setFillColor(sf::Color::Black);
    php.setString("HP: " + std::to_string(m_player->currentHP) + "/" + std::to_string(m_player->maxHP));
    php.setPosition({W * 0.55f, H * 0.62f + 10.f});
    t.draw(php);

    // Message box
    float msgY = H * 0.72f;
    sf::RectangleShape msgBg(sf::Vector2f(W, H - msgY));
    msgBg.setPosition({0.f, msgY});
    msgBg.setFillColor(sf::Color(240, 240, 240));
    msgBg.setOutlineColor(sf::Color::Black);
    msgBg.setOutlineThickness(2.f);
    t.draw(msgBg);

    if (m_state == BattleState::PlayerTurn) {
        drawMoveMenu(t, sz);
    } else {
        // Show current message
        if (m_msgIndex < m_messages.size()) {
            sf::Text msg(m_font);
            msg.setCharacterSize(12);
            msg.setFillColor(sf::Color::Black);
            msg.setString(m_messages[m_msgIndex]);
            msg.setPosition({10.f, msgY + 10.f});
            t.draw(msg);
        }
    }
}

void BattleSystem::drawMoveMenu(sf::RenderTarget& t, sf::Vector2u sz) const {
    if (!m_player) return;
    float W = static_cast<float>(sz.x);
    float H = static_cast<float>(sz.y);
    float msgY = H * 0.72f;

    sf::Text prompt(m_font);
    prompt.setCharacterSize(12);
    prompt.setFillColor(sf::Color::Black);
    prompt.setString("What will " + m_player->getName() + " do?");
    prompt.setPosition({10.f, msgY + 8.f});
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
            sel.setPosition({x - 2.f, y - 1.f});
            sel.setFillColor(sf::Color(180, 200, 255));
            t.draw(sel);
        }

        sf::Text moveText(m_font);
        moveText.setCharacterSize(11);
        moveText.setFillColor(sf::Color::Black);
        moveText.setString(md.name + " PP:" + std::to_string(m_player->currentPP[i]));
        moveText.setPosition({x, y});
        t.draw(moveText);
    }

    // Run option
    sf::Text run(m_font);
    run.setCharacterSize(11);
    run.setFillColor(sf::Color(180, 60, 60));
    run.setString("[R] Run");
    run.setPosition({W - 80.f, msgY + 8.f});
    t.draw(run);
}

void BattleSystem::draw(sf::RenderTarget& target, sf::Vector2u screenSize) const {
    drawBattleBox(target, screenSize);
}

} // namespace pokemon
