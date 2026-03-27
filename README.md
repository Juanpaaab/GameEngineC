# GameEngineC — 2D Game Engine + Pokémon Red Demo

A **C++17** 2D game engine built on **SFML 2.5+**, featuring a clean **Entity-Component-System (ECS)** architecture. Comes bundled with a **Pokémon Red clone demo** that showcases engine capabilities: tilemaps, scene management, turn-based battle mechanics, dialog systems, and more.

---

## Table of Contents

1. [Features](#features)
2. [Architecture](#architecture)
3. [Prerequisites & Build](#prerequisites--build)
4. [Project Structure](#project-structure)
5. [Engine Systems](#engine-systems)
6. [Pokémon Red Demo](#pokémon-red-demo)
7. [Project Status](#project-status)
8. [Roadmap](#roadmap)

---

## Features

**Engine**
- Entity-Component-System (ECS) with type-safe component storage
- Scene management with factory registration and deferred switching
- 2D renderer with z-order sprite/text sorting and camera follow
- Input manager (keyboard + mouse) with per-frame pressed/released queries
- Resource manager with typed caches for textures, fonts, and sounds
- Frame timing with FPS counter and deltaTime clamping
- AABB collision detection with solid pushback and trigger callbacks

**Pokémon Red Demo**
- Tile-based overworld with Pallet Town and Route 1
- Character movement with smooth tile interpolation and collision
- Wild Pokémon encounters in tall grass
- Turn-based battle system with Gen 1 damage formula
  - Speed-based turn order
  - Critical hit mechanic (Gen 1 formula: `speed / 512`)
  - Stat stages (Growl lowers ATK, Leer lowers DEF)
  - Status effects: Paralysis, Poison, Sleep, Burn, Freeze
  - EXP gain and level-up on victory
- Dialog system with character-by-character animation and message queues
- NPC interaction via tile-facing detection
- Map transitions (Pallet Town ↔ Route 1)
- 16 Pokémon species, 18 moves, Gen 1 type chart

---

## Architecture

```
┌────────────────────────────────────────────────────────────┐
│                        engine::Game                        │
│  Main loop: Time → Input → SceneManager → Renderer         │
└──────────┬───────────────────────────────────────┬─────────┘
           │                                       │
    ┌──────▼──────┐                      ┌─────────▼────────┐
    │  SceneManager│                      │    Renderer      │
    │  (scene registry│                   │  (z-sort, camera)│
    │   + switching) │                   └──────────────────┘
    └──────┬──────┘
           │ current scene
    ┌──────▼──────┐
    │   Scene     │
    │  owns World │
    └──────┬──────┘
           │
    ┌──────▼──────────────────────────────────────────────┐
    │                    engine::World (ECS)               │
    │  Entity → Components:                                │
    │    Transform, SpriteComponent, Velocity,             │
    │    BoxCollider, AnimationComponent, Script, Tag, Text│
    │                                                      │
    │  System passes (per frame):                          │
    │    1. Scripts      4. Text sync                      │
    │    2. Physics      5. Animation                      │
    │    3. Sprite sync  6. AABB Collision                 │
    └─────────────────────────────────────────────────────┘
```

---

## Prerequisites & Build

### Dependencies

| Dependency | Version | Notes |
|---|---|---|
| C++ compiler | C++17 | GCC 9+, Clang 9+, MSVC 2019+ |
| CMake | 3.16+ | Build system |
| SFML | 2.5.x | Graphics, window, system, audio |

### Installing SFML

**Windows (vcpkg)**
```bash
vcpkg install sfml:x64-windows
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**Windows (SFML installer)**
1. Download SFML 2.5.x from [sfml-dev.org](https://www.sfml-dev.org/download.php)
2. Set `SFML_DIR` when configuring CMake:
```bash
cmake -S . -B build -DSFML_DIR="C:/SFML-2.5.1/lib/cmake/SFML"
```

**Ubuntu / Debian**
```bash
sudo apt install libsfml-dev
cmake -S . -B build
```

**Arch Linux**
```bash
sudo pacman -S sfml
cmake -S . -B build
```

**macOS (Homebrew)**
```bash
brew install sfml
cmake -S . -B build
```

### Build

```bash
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build build --config Release

# Run the engine demo
./build/examples/demo/EngineDemo

# Run the Pokémon Red clone
./build/games/pokemon/PokemonClone
```

### Portable `.exe` on Windows (MSYS2)

Use the **MSYS2 MinGW64 shell** (not UCRT/Clang unless you build everything with that same toolchain):

```bash
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-sfml

cmake -S . -B build-mingw -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-mingw --target portable_demo
cmake --build build-mingw --target portable_pokemon
```

Portable bundles are generated in:

```text
build-mingw/portable/demo
build-mingw/portable/pokemon
```

Each folder includes the `.exe` plus required runtime DLLs, so you can copy that folder to another Windows machine and run it directly.

---

## Project Structure

```
GameEngineC/
├── CMakeLists.txt          # Root build file: defines engine2d static library
├── README.md
│
├── include/                # Engine public headers
│   ├── Engine.hpp          # Convenience aggregator header
│   ├── core/
│   │   ├── Game.hpp        # Main loop, subsystem access
│   │   ├── Time.hpp        # Delta time, FPS
│   │   └── Window.hpp      # SFML window wrapper
│   ├── ecs/
│   │   ├── Component.hpp   # Base component + type ID system
│   │   ├── Components.hpp  # Built-in components (Transform, Sprite, Velocity…)
│   │   ├── Entity.hpp      # Entity with typed component storage
│   │   └── World.hpp       # ECS world + system update passes
│   ├── input/
│   │   └── InputManager.hpp
│   ├── renderer/
│   │   ├── Camera.hpp      # 2D camera with lerp follow
│   │   └── Renderer.hpp    # Sprite/text rendering with z-order sort
│   ├── resources/
│   │   └── ResourceManager.hpp  # Typed resource caches (texture, font, sound)
│   └── scene/
│       ├── Scene.hpp       # Base scene (owns a World)
│       └── SceneManager.hpp
│
├── src/                    # Engine implementation
│   ├── core/
│   ├── ecs/
│   ├── input/
│   ├── renderer/
│   ├── resources/
│   └── scene/
│
├── examples/
│   └── demo/
│       └── main.cpp        # Minimal engine demo (bouncing sprites)
│
└── games/
    └── pokemon/            # Pokémon Red clone
        ├── CMakeLists.txt
        ├── include/
        │   ├── battle/     BattleSystem.hpp
        │   ├── data/       PokemonData.hpp  (species, moves, type chart)
        │   ├── dialog/     DialogSystem.hpp
        │   ├── player/     Player.hpp
        │   ├── scenes/     BattleScene.hpp, OverworldScene.hpp
        │   └── tilemap/    TileMap.hpp
        └── src/
            ├── main.cpp    PokemonGame entry point
            ├── battle/
            ├── data/
            ├── dialog/
            ├── player/
            ├── scenes/
            └── tilemap/
```

---

## Engine Systems

### ECS (Entity-Component-System)

Entities are lightweight ID holders. Components are plain data structs. The `World` runs fixed system passes each frame.

```cpp
engine::World world;

auto& player = world.createEntity("player");
player.addComponent<engine::Transform>().position = {100.f, 200.f};
player.addComponent<engine::Velocity>().linear    = {50.f, 0.f};
player.addComponent<engine::Script>(
    [](float dt) { /* custom per-entity logic */ }
);

// Query all entities with both Transform and Velocity
world.forEach<engine::Transform, engine::Velocity>([](engine::Entity& e) {
    auto& t = *e.getComponent<engine::Transform>();
    // ...
});
```

**Built-in components:**

| Component | Purpose |
|---|---|
| `Transform` | Position, scale, rotation |
| `SpriteComponent` | SFML sprite + z-order |
| `Velocity` | Linear and angular velocity |
| `BoxCollider` | AABB collider (solid or trigger) |
| `AnimationComponent` | Spritesheet frame animation (multi-row aware) |
| `TextComponent` | SFML text + z-order |
| `Tag` | String identifier |
| `Script` | Per-frame callback + collision callback |

### Scene Management

```cpp
game.getSceneManager().registerScene("menu",
    [](engine::Game& g) { return std::make_unique<MenuScene>(g); });

game.getSceneManager().switchTo("menu"); // deferred to next frame
```

### Camera

```cpp
auto& cam = renderer.getCamera();
cam.follow(playerPosition, 5.f); // lerp factor
cam.setZoom(2.f);
```

### Resource Manager

```cpp
auto& res = game.getResources();
res.textures.load("hero", "assets/hero.png");
sf::Texture* tex = res.textures.get("hero");
```

### Input Manager

```cpp
auto& input = game.getInput();
if (input.isKeyPressed(sf::Keyboard::Space))  { /* just pressed this frame */ }
if (input.isKeyDown(sf::Keyboard::Left))       { /* held down */ }
if (input.isKeyReleased(sf::Keyboard::Return)) { /* just released */ }
```

---

## Pokémon Red Demo

### Controls

| Key | Action |
|---|---|
| Arrow keys / WASD | Move player |
| Z / Enter | Interact with signs and NPCs; confirm in battle |
| R / Escape | Run from battle |
| ↑ ↓ ← → | Navigate battle move menu |
| Escape (overworld) | Quit |

### Gameplay

1. **Overworld** — Explore Pallet Town. Walk into **tall grass** (dark green) to trigger wild battles.
2. **Route 1** — Walk north through the exit paths (brown Door tiles) at the top of Pallet Town. Route 1 has higher-level wild Pokémon.
3. **Battles** — Select a move from the menu. The faster Pokémon attacks first. Defeat the wild Pokémon to gain EXP and potentially level up.
4. **NPCs** — Face a character and press Z/Enter to read their dialog.

### Pokémon Available

Starters: Charmander (your default starter, level 5).

Wild encounters:
- **Pallet Town tall grass**: Rattata, Pidgey, Caterpie (levels 3–6)
- **Route 1**: Pidgey, Rattata, Weedle (levels 3–7)

All 16 starter-line Pokémon are in the database (Bulbasaur, Ivysaur, Venusaur, Charmander, Charmeleon, Charizard, Squirtle, Wartortle, Blastoise, Caterpie, Metapod, Butterfree, Weedle, Pidgey, Rattata, Pikachu, Eevee).

### Battle Mechanics

- **Turn order**: Pokémon with higher Speed acts first. Paralysis halves Speed for ordering.
- **Damage**: Gen 1 formula `((2*L/5+2) * Power * Atk/Def / 50 + 2) * STAB * Type * Crit * Random`
- **Critical hits**: Probability = attacker Speed / 512. Doubles damage.
- **Stat stages**: Growl lowers enemy ATK, Leer lowers enemy DEF (±6 stage cap).
- **Status effects**:
  - **Paralysis** — 25% chance to skip turn, Speed halved for turn order
  - **Poison** — Lose 1/8 max HP at end of each turn
  - **Burn** — Lose 1/8 max HP at end of each turn; Attack halved
  - **Sleep** — Skip turns until randomly waking up
  - **Freeze** — Skip turns; 20% chance to thaw per turn
- **EXP**: Gain `enemy level × 10` EXP on victory. Level up when EXP threshold reached.

---

## Project Status

| System | Status | Notes |
|---|---|---|
| ECS core (Entity/World) | ✅ Working | AABB collision implemented |
| Renderer (z-sort, camera) | ✅ Working | No batch rendering |
| Input manager | ✅ Working | Keyboard + mouse; no gamepad |
| Scene manager | ✅ Working | Deferred switching |
| Resource manager | ✅ Working | Texture, font, sound caches |
| Animation system | ✅ Working | Multi-row spritesheet aware |
| Tilemap (Pallet Town) | ✅ Working | Placeholder colors (no sprites) |
| Tilemap (Route 1) | ✅ Working | North of Pallet Town |
| Player movement | ✅ Working | Tile-based with smooth lerp |
| Wild encounters | ✅ Working | Pallet Town + Route 1 pools |
| Battle system | ✅ Working | Speed order, crits, stat stages, status, EXP |
| Dialog system | ✅ Working | Queued messages, NPC interaction |
| NPC interaction | ✅ Working | 2 NPCs per map |
| Map transitions | ✅ Working | Pallet Town ↔ Route 1 |
| Pokémon database | 🟡 Partial | 16 species, 18 moves (Gen 1 has 151/165) |
| Sprite graphics | ❌ Placeholder | Colored rectangles only |
| Audio | ❌ Missing | No music or sound effects |
| Save / load | ❌ Missing | Restarts fresh each run |
| Trainer battles | ❌ Missing | Wild encounters only |

---

## Roadmap

The following features are planned but not yet implemented:

- [ ] **Full Pokédex** — All 151 Gen 1 species with correct base stats and learnsets
- [ ] **Full move database** — All 165 Gen 1 moves with correct effects
- [ ] **Sprite graphics** — Replace colored rectangles with actual Pokémon/tile sprites
- [ ] **Audio** — Background music (Pallet Town, battle themes) and sound effects
- [ ] **Save / load system** — Persist player progress to disk
- [ ] **More maps** — Viridian City, Pewter City, Viridian Forest, Routes 2–24
- [ ] **Indoor areas** — Building interiors, Pokémon Center, PokéMart
- [ ] **Trainer battles** — NPC trainers, Gym Leaders, Rival
- [ ] **Pokémon Center** — Full HP/Status restore
- [ ] **PokéMart** — Buy Poké Balls, Potions, Antidotes
- [ ] **Poké Balls mechanic** — Catch wild Pokémon
- [ ] **Party management** — Switch active Pokémon mid-battle
- [ ] **Evolution system** — Level-up and stone evolutions
- [ ] **IV / EV system** — Full Gen 1 stat formula
- [ ] **Bag / item system** — Potions, status heals, TMs
- [ ] **Gamepad support** — Controller input via SFML joystick API
- [ ] **Camera bounds clamping** — Prevent camera from showing outside map
- [ ] **Debug overlay** — Entity count, collision boxes, frame graph

---

## License

This project is for educational purposes. Pokémon is a registered trademark of The Pokémon Company / Nintendo. This is an unofficial fan-made clone with no affiliation.


Set-Location "c:\Secundarios\Gaming dev\GameEngineC" ; $env:Path += ";C:\msys64\mingw64\bin" ; Start-Process -FilePath ".\build\games\pokemon\pokemon.exe" ; Start-Sleep -Milliseconds 900 ; Get-Process pokemon -ErrorAction SilentlyContinue | Select-Object ProcessName,Id