#include "core/Game.hpp"

namespace engine {

Game::Game(const WindowConfig& config)
    : m_window(config)
    , m_sceneManager(*this)
{
    m_renderer.setWindow(&m_window);
}

void Game::run() {
    m_running = true;
    onInit();

    while (m_window.isOpen() && m_running) {
        m_time.update();
        float dt = m_time.getDeltaTime();

        processEvents();
        update(dt);
        render();
    }

    onShutdown();
}

void Game::quit() {
    m_running = false;
    m_window.close();
}

void Game::processEvents() {
    m_input.beginFrame();
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            quit();
        m_input.handleEvent(*event);
    }
}

void Game::update(float dt) {
    m_sceneManager.update(dt);
    onUpdate(dt);
}

void Game::render() {
    m_window.clear();
    m_sceneManager.render();
    onRender();
    m_window.display();
}

} // namespace engine
