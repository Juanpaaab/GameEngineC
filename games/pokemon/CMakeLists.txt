cmake_minimum_required(VERSION 3.16)
project(PokemonClone VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Assume engine2d target is available from parent or via find_package
# When built standalone, add engine as subdirectory
if(NOT TARGET engine2d)
    set(ENGINE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    add_subdirectory(${ENGINE_ROOT} ${CMAKE_BINARY_DIR}/engine)
endif()

set(POKEMON_SOURCES
    src/data/PokemonData.cpp
    src/tilemap/TileMap.cpp
    src/player/Player.cpp
    src/dialog/DialogSystem.cpp
    src/battle/BattleSystem.cpp
    src/scenes/OverworldScene.cpp
    src/scenes/BattleScene.cpp
    src/main.cpp
)

add_executable(pokemon ${POKEMON_SOURCES})

target_include_directories(pokemon
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pokemon PRIVATE engine2d)
