#pragma once
#include "core/Window.hpp"
#include "core/Time.hpp"
#include "input/InputManager.hpp"
#include "scene/SceneManager.hpp"
#include "resources/ResourceManager.hpp"
#include "renderer/Renderer.hpp"

namespace engine {

class Game {
public:
    explicit Game(const WindowConfig& config = {});
    virtual ~Game() = default;

    void run();
    void quit();

    Window&          getWindow()          { return m_window; }
    Time&            getTime()            { return m_time; }
    InputManager&    getInput()           { return m_input; }
    SceneManager&    getSceneManager()    { return m_sceneManager; }
    ResourceManager& getResourceManager() { return m_resources; }
    Renderer&        getRenderer()        { return m_renderer; }

protected:
    virtual void onInit()   {}
    virtual void onUpdate(float dt) {}
    virtual void onRender() {}
    virtual void onShutdown() {}

private:
    void processEvents();
    void update(float dt);
    void render();

    Window          m_window;
    Time            m_time;
    InputManager    m_input;
    SceneManager    m_sceneManager;
    ResourceManager m_resources;
    Renderer        m_renderer;
    bool            m_running = false;
};

} // namespace engine
