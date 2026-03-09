#include "data/PokemonData.hpp"
#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace pokemon {

// ---- Type names ----------------------------------------------------------
const char* typeName(Type t) {
    static const char* names[] = {
        "Normal","Fire","Water","Grass","Electric","Ice",
        "Fighting","Poison","Ground","Flying","Psychic",
        "Bug","Rock","Ghost","Dragon"
    };
    return names[static_cast<int>(t)];
}

// Simplified Gen-1 type chart (only the interesting entries)
float typeChart(Type atk, Type def) {
    using T = Type;
    // Super effective pairs
    static const std::vector<std::pair<std::pair<T,T>,float>> table = {
        {{T::Fire,   T::Grass},  2.f}, {{T::Fire,   T::Ice},    2.f},
        {{T::Fire,   T::Bug},    2.f}, {{T::Fire,   T::Water},  0.5f},
        {{T::Fire,   T::Rock},   0.5f},{{T::Fire,   T::Fire},   0.5f},
        {{T::Water,  T::Fire},   2.f}, {{T::Water,  T::Ground}, 2.f},
        {{T::Water,  T::Rock},   2.f}, {{T::Water,  T::Grass},  0.5f},
        {{T::Water,  T::Water},  0.5f},
        {{T::Grass,  T::Water},  2.f}, {{T::Grass,  T::Ground}, 2.f},
        {{T::Grass,  T::Rock},   2.f}, {{T::Grass,  T::Fire},   0.5f},
        {{T::Grass,  T::Grass},  0.5f},{{T::Grass,  T::Poison}, 0.5f},
        {{T::Electric,T::Water}, 2.f}, {{T::Electric,T::Flying},2.f},
        {{T::Electric,T::Grass}, 0.5f},{{T::Electric,T::Ground},0.f},
        {{T::Psychic,T::Fighting},2.f},{{T::Psychic,T::Poison}, 2.f},
        {{T::Ghost,  T::Psychic},0.f}, {{T::Ghost,  T::Normal}, 0.f},
        {{T::Normal, T::Rock},   0.5f},{{T::Normal, T::Ghost},  0.f},
        {{T::Fighting,T::Normal},2.f}, {{T::Fighting,T::Rock},  2.f},
        {{T::Fighting,T::Ghost}, 0.f},
        {{T::Poison, T::Grass},  2.f}, {{T::Poison, T::Rock},   0.5f},
        {{T::Ground, T::Fire},   2.f}, {{T::Ground, T::Electric},2.f},
        {{T::Ground, T::Rock},   2.f}, {{T::Ground, T::Flying}, 0.f},
        {{T::Flying, T::Grass},  2.f}, {{T::Flying, T::Fighting},2.f},
        {{T::Flying, T::Rock},   0.5f},
        {{T::Bug,    T::Grass},  2.f}, {{T::Bug,    T::Poison}, 2.f},
        {{T::Bug,    T::Fire},   0.5f},
        {{T::Rock,   T::Fire},   2.f}, {{T::Rock,   T::Flying}, 2.f},
        {{T::Rock,   T::Bug},    2.f}, {{T::Rock,   T::Ice},    2.f},
        {{T::Ice,    T::Grass},  2.f}, {{T::Ice,    T::Ground}, 2.f},
        {{T::Ice,    T::Flying}, 2.f}, {{T::Ice,    T::Dragon}, 2.f},
        {{T::Ice,    T::Water},  0.5f},
    };
    for (auto& [key, val] : table)
        if (key.first == atk && key.second == def) return val;
    return 1.f;
}

// ---- Move database -------------------------------------------------------
static const std::unordered_map<int, MoveData> s_moves = {
    {(int)MoveID::Tackle,      {"Tackle",      Type::Normal,   35, 95, 35, false}},
    {(int)MoveID::Scratch,     {"Scratch",     Type::Normal,   40, 100,35, false}},
    {(int)MoveID::Growl,       {"Growl",       Type::Normal,   0,  100,40, false}},
    {(int)MoveID::Leer,        {"Leer",        Type::Normal,   0,  100,30, false}},
    {(int)MoveID::Ember,       {"Ember",       Type::Fire,     40, 100,25, true }},
    {(int)MoveID::WaterGun,    {"Water Gun",   Type::Water,    40, 100,25, true }},
    {(int)MoveID::VineWhip,    {"Vine Whip",   Type::Grass,    35, 100,10, false}},
    {(int)MoveID::Thundershock,{"Thundershock",Type::Electric, 40, 100,30, true }},
    {(int)MoveID::Confusion,   {"Confusion",   Type::Psychic,  50, 100,25, true }},
    {(int)MoveID::QuickAttack, {"Quick Attack",Type::Normal,   40, 100,30, false}},
    {(int)MoveID::RazorLeaf,   {"Razor Leaf",  Type::Grass,    55, 95, 25, false}},
    {(int)MoveID::Flamethrower,{"Flamethrower",Type::Fire,     95, 100,15, true }},
    {(int)MoveID::Surf,        {"Surf",        Type::Water,    95, 100,15, true }},
    {(int)MoveID::Thunderbolt, {"Thunderbolt", Type::Electric, 95, 100,15, true }},
    {(int)MoveID::Slam,        {"Slam",        Type::Normal,   80, 75, 20, false}},
    {(int)MoveID::Bite,        {"Bite",        Type::Normal,   60, 100,25, false}},
};

const MoveData& getMoveData(MoveID id) {
    auto it = s_moves.find(static_cast<int>(id));
    if (it == s_moves.end()) {
        static MoveData empty{"???", Type::Normal, 0, 0, 0, false};
        return empty;
    }
    return it->second;
}

// ---- Species database ----------------------------------------------------
using LS = std::vector<std::pair<uint8_t,MoveID>>;
static SpeciesData makeSD(uint16_t id, const char* name, Type t1, Type t2,
                          uint8_t hp,uint8_t atk,uint8_t def,uint8_t spe,uint8_t spc,
                          LS learnset, uint32_t color) {
    SpeciesData sd;
    sd.id=id; sd.name=name; sd.type1=t1; sd.type2=t2;
    sd.baseHP=hp; sd.baseAtk=atk; sd.baseDef=def; sd.baseSpe=spe; sd.baseSpc=spc;
    sd.learnset=std::move(learnset); sd.color=color;
    return sd;
}

static std::unordered_map<int,SpeciesData> buildSpeciesDB() {
    std::unordered_map<int,SpeciesData> db;
    auto add = [&](Species s, SpeciesData d){ db[(int)s] = std::move(d); };
    using M = MoveID;
    add(Species::Bulbasaur, makeSD(1,"Bulbasaur",Type::Grass,Type::Poison,  45,49,49,45,65,{{1,M::Tackle},{3,M::Growl},{7,M::VineWhip}},0x78C850FFu));
    add(Species::Ivysaur,   makeSD(2,"Ivysaur",  Type::Grass,Type::Poison,  60,62,63,60,80,{{1,M::Tackle},{1,M::Growl},{7,M::VineWhip},{13,M::RazorLeaf}},0x78C850FFu));
    add(Species::Venusaur,  makeSD(3,"Venusaur", Type::Grass,Type::Poison,  80,82,83,80,100,{{1,M::Tackle},{1,M::Growl},{7,M::VineWhip},{13,M::RazorLeaf}},0x78C850FFu));
    add(Species::Charmander,makeSD(4,"Charmander",Type::Fire,Type::Fire,    39,52,43,65,50,{{1,M::Scratch},{1,M::Growl},{7,M::Ember}},0xF08030FFu));
    add(Species::Charmeleon,makeSD(5,"Charmeleon",Type::Fire,Type::Fire,    58,64,58,80,65,{{1,M::Scratch},{1,M::Growl},{7,M::Ember},{15,M::Slam}},0xF08030FFu));
    add(Species::Charizard, makeSD(6,"Charizard", Type::Fire,Type::Flying,  78,84,78,100,85,{{1,M::Scratch},{1,M::Growl},{7,M::Ember},{15,M::Flamethrower}},0xF08030FFu));
    add(Species::Squirtle,  makeSD(7,"Squirtle",  Type::Water,Type::Water,  44,48,65,43,50,{{1,M::Tackle},{1,M::Growl},{7,M::WaterGun}},0x6890F0FFu));
    add(Species::Wartortle, makeSD(8,"Wartortle", Type::Water,Type::Water,  59,63,80,58,65,{{1,M::Tackle},{1,M::Growl},{7,M::WaterGun},{13,M::Bite}},0x6890F0FFu));
    add(Species::Blastoise, makeSD(9,"Blastoise", Type::Water,Type::Water,  79,83,100,78,85,{{1,M::Tackle},{1,M::Growl},{7,M::WaterGun},{13,M::Bite}},0x6890F0FFu));
    add(Species::Caterpie,  makeSD(10,"Caterpie", Type::Bug,  Type::Bug,    45,30,35,45,20,{{1,M::Tackle}},0xA8B820FFu));
    add(Species::Pidgey,    makeSD(16,"Pidgey",   Type::Normal,Type::Flying,40,45,40,56,35,{{1,M::Tackle},{5,M::Growl},{9,M::QuickAttack}},0xA8A878FFu));
    add(Species::Rattata,   makeSD(19,"Rattata",  Type::Normal,Type::Normal,30,56,35,72,25,{{1,M::Tackle},{1,M::Growl},{7,M::QuickAttack}},0xA8A8A8FFu));
    add(Species::Pikachu,   makeSD(25,"Pikachu",  Type::Electric,Type::Electric,35,55,40,90,50,{{1,M::Thundershock},{1,M::Growl},{9,M::QuickAttack}},0xF8D030FFu));
    add(Species::Eevee,     makeSD(133,"Eevee",   Type::Normal,Type::Normal,55,55,50,55,65,{{1,M::Tackle},{1,M::Growl},{9,M::QuickAttack},{23,M::Bite}},0xC8A878FFu));
    return db;
}
static const std::unordered_map<int,SpeciesData> s_species = buildSpeciesDB();

const SpeciesData& getSpeciesData(Species s) {
    auto it = s_species.find(static_cast<int>(s));
    if (it == s_species.end()) {
        static SpeciesData empty{0,"???",Type::Normal,Type::Normal,1,1,1,1,1,{},0xFF0000FF};
        return empty;
    }
    return it->second;
}

// ---- PokemonInstance -------------------------------------------------------
const std::string& PokemonInstance::getName() const {
    if (!nickname.empty()) return nickname;
    return getSpeciesData(species).name;
}

void PokemonInstance::recalcStats() {
    const auto& base = getSpeciesData(species);
    // Gen 1 stat formula: ((Base + IV) * 2 + StatExp^0.5) * Level / 100 + 5
    // Simplified (IVs=0, StatExp=0):
    auto calc = [&](int b) -> int {
        return static_cast<int>((b * 2 * level) / 100.0 + 5);
    };
    auto calcHP = [&](int b) -> int {
        return static_cast<int>((b * 2 * level) / 100.0 + level + 10);
    };
    maxHP   = calcHP(base.baseHP);
    attack  = calc(base.baseAtk);
    defense = calc(base.baseDef);
    speed   = calc(base.baseSpe);
    special = calc(base.baseSpc);
}

void PokemonInstance::init(Species s, uint8_t lvl, const std::array<MoveID,MAX_MOVES>& moveSet) {
    species = s;
    level   = lvl;
    moves   = moveSet;
    for (int i = 0; i < MAX_MOVES; ++i)
        currentPP[i] = getMoveData(moves[i]).maxPP;
    recalcStats();
    currentHP = maxHP;
}

} // namespace pokemon
