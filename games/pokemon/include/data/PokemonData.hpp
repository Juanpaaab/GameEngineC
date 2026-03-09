#pragma once
#include <string>
#include <vector>
#include <array>
#include <cstdint>

namespace pokemon {

// ---- Types ---------------------------------------------------------------
enum class Type : uint8_t {
    Normal=0, Fire, Water, Grass, Electric, Ice,
    Fighting, Poison, Ground, Flying, Psychic,
    Bug, Rock, Ghost, Dragon, COUNT
};

const char* typeName(Type t);

// type effectiveness multiplier index: [attacking][defending]
float typeChart(Type attacking, Type defending);

// ---- Moves ---------------------------------------------------------------
struct MoveData {
    std::string name;
    Type        type;
    uint8_t     power;    // 0 = status
    uint8_t     accuracy; // 0-100
    uint8_t     maxPP;
    bool        isSpecial;
};

// Move IDs
enum class MoveID : uint16_t {
    None = 0,
    Tackle, Scratch, Growl, Leer,
    Ember, WaterGun, VineWhip,
    Thundershock, Confusion,
    QuickAttack, RazorLeaf,
    Flamethrower, Surf, Thunderbolt,
    Slam, Bite,
    COUNT
};

const MoveData& getMoveData(MoveID id);

// ---- Species -------------------------------------------------------------
struct SpeciesData {
    uint16_t    id;
    std::string name;
    Type        type1;
    Type        type2;     // == type1 if single type
    // Base stats
    uint8_t  baseHP;
    uint8_t  baseAtk;
    uint8_t  baseDef;
    uint8_t  baseSpe;
    uint8_t  baseSpc;
    // Learnset: (level, MoveID)
    std::vector<std::pair<uint8_t, MoveID>> learnset;
    // Sprite color (used to draw placeholder)
    uint32_t color; // RGBA
};

enum class Species : uint16_t {
    None = 0,
    Bulbasaur=1, Ivysaur, Venusaur,
    Charmander=4, Charmeleon, Charizard,
    Squirtle=7, Wartortle, Blastoise,
    Caterpie=10, Metapod, Butterfree,
    Pidgey=16,
    Rattata=19,
    Pikachu=25,
    Eevee=133,
    COUNT
};

const SpeciesData& getSpeciesData(Species s);

// ---- Pokemon instance ----------------------------------------------------
struct PokemonInstance {
    Species   species   = Species::None;
    std::string nickname;
    uint8_t   level     = 1;
    uint32_t  exp       = 0;

    int  maxHP  = 0;
    int  currentHP = 0;
    int  attack = 0;
    int  defense= 0;
    int  speed  = 0;
    int  special= 0;

    static const int MAX_MOVES = 4;
    std::array<MoveID, MAX_MOVES> moves    = {MoveID::None,MoveID::None,MoveID::None,MoveID::None};
    std::array<uint8_t, MAX_MOVES> currentPP = {0,0,0,0};

    bool isFainted() const { return currentHP <= 0; }
    const std::string& getName() const;

    // Compute stats from species base stats and level
    void recalcStats();
    void init(Species s, uint8_t lvl, const std::array<MoveID,MAX_MOVES>& moveSet);
};

} // namespace pokemon
