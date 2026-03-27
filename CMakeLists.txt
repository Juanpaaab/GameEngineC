cmake_minimum_required(VERSION 3.16)
project(GameEngine2D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENGINE_WINDOWS_PORTABLE "Enable portable bundle targets for Windows/MSYS2" ON)

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

if(WIN32 AND MINGW AND ENGINE_WINDOWS_PORTABLE)
    set(PORTABLE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyRuntimeDeps.cmake")
    get_filename_component(MINGW_TOOLCHAIN_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    function(create_portable_target executable_target portable_name)
        set(portable_dir "${CMAKE_BINARY_DIR}/portable/${portable_name}")

        add_custom_target(portable_${portable_name}
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${portable_dir}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${portable_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${executable_target}>" "${portable_dir}/"
            COMMAND ${CMAKE_COMMAND}
                -Dexe=$<TARGET_FILE:${executable_target}>
                -Ddest=${portable_dir}
                -Dtoolchain_bin=${MINGW_TOOLCHAIN_BIN_DIR}
                -P "${PORTABLE_SCRIPT}"
            DEPENDS ${executable_target}
            COMMENT "Building portable bundle for ${portable_name}"
            VERBATIM
        )
    endfunction()

    create_portable_target(demo demo)

    if(TARGET pokemon)
        create_portable_target(pokemon pokemon)
    endif()
endif()

# Install rules
install(TARGETS engine2d
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)
