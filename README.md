# GameEngine2D — Motor de videojuegos en C++

Motor de videojuegos 2D escrito en C++17 con SFML. Incluye un sistema de entidades y componentes (ECS), gestión de escenas, entrada, recursos y renderizado. Como ejemplo de uso se incluye un clon de Pokémon Rojo.

## Características

- **Motor ECS** — Entidades, componentes y mundo con iteración por tipo de componente.
- **Gestión de escenas** — Registro y cambio de escenas con transiciones limpias.
- **Renderer** — Orden de capas (z-order), cámara con seguimiento suave, primitivas de depuración.
- **InputManager** — Estado de teclado y ratón por fotograma (presionado, suelto, mantenido).
- **ResourceManager** — Caché de texturas, fuentes y sonidos con carga lazy.
- **Bucle de juego** — Delta time, contador de FPS.

## Requisitos

| Herramienta | Versión mínima |
|-------------|---------------|
| CMake       | 3.16          |
| Compilador  | C++17 (GCC 9+, Clang 10+, MSVC 19.17+) |
| SFML        | 2.5           |

### Instalar dependencias (Ubuntu / Debian)

```bash
sudo apt-get install libsfml-dev cmake build-essential
```

### Instalar dependencias (macOS)

```bash
brew install sfml cmake
```

## Compilar

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

Los ejecutables quedan en `build/`:

| Ejecutable        | Descripción                        |
|-------------------|------------------------------------|
| `demo`            | Demo interactivo del motor         |
| `games/pokemon/pokemon` | Clon de Pokémon Rojo          |

## Ejecutar el demo

```bash
./build/demo
```

Controles del demo:

| Tecla            | Acción           |
|------------------|------------------|
| W / ↑            | Mover arriba     |
| S / ↓            | Mover abajo      |
| A / ←            | Mover izquierda  |
| D / →            | Mover derecha    |
| Q                | Rotar izquierda  |
| E                | Rotar derecha    |
| ESC              | Salir            |

## Ejecutar el juego Pokémon

```bash
./build/games/pokemon/pokemon
```

Controles del juego:

| Tecla    | Acción                               |
|----------|--------------------------------------|
| W/A/S/D  | Mover al jugador                     |
| Enter    | Confirmar / interactuar              |
| Backspace | Retroceder en menús                  |
| ESC      | Salir                                |

## Estructura del proyecto

```
GameEngineC/
├── CMakeLists.txt          # Build principal
├── include/                # Cabeceras públicas del motor
│   ├── Engine.hpp          # Include único para el motor completo
│   ├── core/               # Game, Window, Time
│   ├── ecs/                # Component, Entity, World, Components
│   ├── input/              # InputManager
│   ├── renderer/           # Renderer, Camera
│   ├── resources/          # ResourceManager
│   └── scene/              # Scene, SceneManager
├── src/                    # Implementaciones del motor
│   ├── core/
│   ├── ecs/
│   ├── input/
│   ├── renderer/
│   ├── resources/
│   └── scene/
├── examples/
│   └── demo/               # Demo interactivo del motor
└── games/
    └── pokemon/            # Clon de Pokémon Rojo
        ├── include/        # Cabeceras del juego
        └── src/            # Fuentes del juego
```

## Crear un juego nuevo

1. Crea una carpeta en `games/` para tu juego.
2. Añade un `CMakeLists.txt` similar al de `games/pokemon/CMakeLists.txt`.
3. Registra el subdirectorio en el `CMakeLists.txt` raíz.
4. Crea tu clase de juego heredando de `engine::Game` y tus escenas heredando de `engine::Scene`.

### Ejemplo mínimo

```cpp
#include "Engine.hpp"

class MiEscena : public engine::Scene {
public:
    explicit MiEscena(engine::Game& game) : engine::Scene(game) {}

    void onEnter() override {
        auto& e = m_world.createEntity("Jugador");
        e.addComponent<engine::Transform>().position = {400.f, 300.f};
        // ...
    }
};

class MiJuego : public engine::Game {
public:
    MiJuego() : engine::Game({800, 600, "Mi Juego", 60}) {}

protected:
    void onInit() override {
        getSceneManager().registerScene("main", [](engine::Game& g) {
            return std::make_unique<MiEscena>(g);
        });
        getSceneManager().switchTo("main");
    }
};

int main() {
    MiJuego juego;
    juego.run();
}
```

## Componentes disponibles

| Componente          | Descripción                                         |
|---------------------|-----------------------------------------------------|
| `Transform`         | Posición, escala y rotación 2D                      |
| `SpriteComponent`   | Sprite con orden de capa                            |
| `Velocity`          | Velocidad lineal y angular (integración automática) |
| `BoxCollider`       | Colisionador AABB                                   |
| `Tag`               | Etiqueta de texto libre                             |
| `Script`            | Callback `onUpdate(float dt)` por entidad           |
| `AnimationComponent` | Animación por spritesheet                         |
| `TextComponent`     | Texto `sf::Text` con orden de capa                 |

## Licencia

Proyecto de uso educativo. SFML está licenciado bajo [zlib/png license](https://www.sfml-dev.org/license.php).
