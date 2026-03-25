cmake_minimum_required(VERSION 3.16)
project(GameEngine2D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SFML
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

# Engine library sources
set(ENGINE_SOURCES
    src/core/Window.cpp
    src/core/Game.cpp
    src/core/Time.cpp
    src/input/InputManager.cpp
    src/ecs/Entity.cpp
    src/ecs/World.cpp
    src/scene/Scene.cpp
    src/scene/SceneManager.cpp
    src/resources/ResourceManager.cpp
    src/renderer/Renderer.cpp
    src/renderer/Camera.cpp
)

# Build engine as static library
add_library(engine2d STATIC ${ENGINE_SOURCES})

target_include_directories(engine2d
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(engine2d
    PUBLIC
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)

# Demo executable
add_executable(demo examples/demo/main.cpp)
target_link_libraries(demo PRIVATE engine2d)

# Pokemon game
add_subdirectory(games/pokemon)

# Install rules
install(TARGETS engine2d
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)
