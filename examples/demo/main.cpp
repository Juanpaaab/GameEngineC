#include "Engine.hpp"
#include <cmath>

// ---- Demo Scene --------------------------------------------------------

class DemoScene : public engine::Scene {
public:
    explicit DemoScene(engine::Game& game) : engine::Scene(game) {}

    void onEnter() override {
        auto& res = m_game.getResourceManager();

        // Create a simple colored square as a "player"
        m_playerTex.create(32, 32);
        std::vector<sf::Uint8> pixels(32 * 32 * 4);
        for (int i = 0; i < 32 * 32; ++i) {
            pixels[i*4+0] = 100; // R
            pixels[i*4+1] = 180; // G
            pixels[i*4+2] = 255; // B
            pixels[i*4+3] = 255; // A
        }
        m_playerTex.update(pixels.data());

        // Load font (built-in fallback: just draw a shape if font not found)
        m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
        if (!m_fontLoaded)
            m_fontLoaded = m_font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");

        // --- Player entity ---
        auto& player = m_world.createEntity("Player");
        player.addComponent<engine::Transform>().position = {384.f, 284.f};
        auto& sc = player.addComponent<engine::SpriteComponent>(m_playerTex);
        sc.sprite.setOrigin(16.f, 16.f);
        sc.zOrder = 1;
        player.addComponent<engine::Velocity>();
        player.addComponent<engine::BoxCollider>().size = {32.f, 32.f};
        player.addComponent<engine::Tag>("player");

        // Script: player movement
        m_playerID = player.getID();
        player.addComponent<engine::Script>([this](float dt) {
            auto* entity = m_world.getEntity(m_playerID);
            if (!entity) return;
            auto& vel  = *entity->getComponent<engine::Velocity>();
            auto& inp  = m_game.getInput();

            const float speed = 200.f;
            vel.linear = {0.f, 0.f};
            if (inp.isKeyDown(sf::Keyboard::W) || inp.isKeyDown(sf::Keyboard::Up))    vel.linear.y -= speed;
            if (inp.isKeyDown(sf::Keyboard::S) || inp.isKeyDown(sf::Keyboard::Down))  vel.linear.y += speed;
            if (inp.isKeyDown(sf::Keyboard::A) || inp.isKeyDown(sf::Keyboard::Left))  vel.linear.x -= speed;
            if (inp.isKeyDown(sf::Keyboard::D) || inp.isKeyDown(sf::Keyboard::Right)) vel.linear.x += speed;

            // Rotate
            if (inp.isKeyDown(sf::Keyboard::Q)) vel.angular = -90.f;
            else if (inp.isKeyDown(sf::Keyboard::E)) vel.angular = 90.f;
            else vel.angular = 0.f;
        });

        // --- Some obstacle entities ---
        createObstacle({150.f, 150.f}, {48.f, 48.f}, sf::Color(220, 80, 80));
        createObstacle({600.f, 400.f}, {64.f, 24.f}, sf::Color(80, 220, 80));
        createObstacle({300.f, 450.f}, {24.f, 64.f}, sf::Color(220, 180, 60));

        // --- HUD text ---
        if (m_fontLoaded) {
            auto& hud = m_world.createEntity("HUD");
            auto& t = hud.addComponent<engine::Transform>();
            t.position = {10.f, 10.f};
            auto& tc = hud.addComponent<engine::TextComponent>();
            tc.text.setFont(m_font);
            tc.text.setCharacterSize(16);
            tc.text.setFillColor(sf::Color::White);
            tc.text.setString("WASD/Arrows: Move  Q/E: Rotate  ESC: Quit");
            tc.zOrder = 10;
            m_hudID = hud.getID();
        }

        // Camera follows player
        auto size = m_game.getWindow().getSize();
        m_game.getRenderer().getCamera() = engine::Camera(
            static_cast<float>(size.x), static_cast<float>(size.y));
    }

    void update(float dt) override {
        engine::Scene::update(dt);

        // Camera follows player smoothly
        auto* player = m_world.getEntity(m_playerID);
        if (player) {
            auto& t = *player->getComponent<engine::Transform>();
            m_game.getRenderer().getCamera().follow(t.position, 5.f * dt);
        }

        // Update HUD
        if (m_fontLoaded) {
            auto* hud = m_world.getEntity(m_hudID);
            if (hud) {
                auto& tc = *hud->getComponent<engine::TextComponent>();
                int fps = m_game.getTime().getFPS();
                tc.text.setString("WASD/Arrows: Move  Q/E: Rotate  ESC: Quit  |  FPS: " + std::to_string(fps));
            }
        }
    }

    void render() override {
        // Render world (sprites)
        engine::Scene::render();

        // Draw grid lines using renderer (in world space)
        auto& renderer = m_game.getRenderer();
        m_game.getWindow().setView(renderer.getCamera().getView());
        for (int x = 0; x <= 800; x += 64)
            renderer.drawLine({(float)x, 0.f}, {(float)x, 600.f},
                              sf::Color(50, 50, 50));
        for (int y = 0; y <= 600; y += 64)
            renderer.drawLine({0.f, (float)y}, {800.f, (float)y},
                              sf::Color(50, 50, 50));

        // HUD is in screen space, reset view
        m_game.getWindow().setView(m_game.getWindow().getDefaultView());
        if (m_fontLoaded) {
            auto* hud = m_world.getEntity(m_hudID);
            if (hud)
                renderer.drawText(hud->getComponent<engine::TextComponent>()->text);
        }
    }

private:
    void createObstacle(sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
        sf::Texture* tex = new sf::Texture();
        tex->create((unsigned)size.x, (unsigned)size.y);
        std::vector<sf::Uint8> pixels((int)size.x * (int)size.y * 4);
        for (std::size_t i = 0; i < pixels.size(); i += 4) {
            pixels[i+0] = color.r;
            pixels[i+1] = color.g;
            pixels[i+2] = color.b;
            pixels[i+3] = 255;
        }
        tex->update(pixels.data());
        m_textures.emplace_back(tex);

        auto& e = m_world.createEntity("Obstacle");
        auto& tr = e.addComponent<engine::Transform>();
        tr.position = pos;
        auto& sc = e.addComponent<engine::SpriteComponent>(*tex);
        sc.zOrder = 0;
        e.addComponent<engine::BoxCollider>().size = size;
    }

    sf::Texture              m_playerTex;
    std::vector<std::unique_ptr<sf::Texture>> m_textures;
    sf::Font                 m_font;
    bool                     m_fontLoaded = false;
    engine::EntityID         m_playerID   = engine::NULL_ENTITY;
    engine::EntityID         m_hudID      = engine::NULL_ENTITY;
};

// ---- Main Game ---------------------------------------------------------

class DemoGame : public engine::Game {
public:
    DemoGame() : engine::Game({800, 600, "Game Engine 2D - Demo", 60, false}) {}

protected:
    void onInit() override {
        getSceneManager().registerScene("demo", [](engine::Game& g) {
            return std::make_unique<DemoScene>(g);
        });
        getSceneManager().switchTo("demo");
    }

    void onUpdate(float /*dt*/) override {
        if (getInput().isKeyPressed(sf::Keyboard::Escape))
            quit();
    }
};

int main() {
    DemoGame game;
    game.run();
    return 0;
}
