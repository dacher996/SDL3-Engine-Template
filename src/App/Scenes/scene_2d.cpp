#include "App/Scenes/scene_2d.h"

#include "Engine/Systems/audio_system.h"
#include "Engine/Core/app.h"
#include "Engine/Core/components.h"
#include "Engine/Core/Input/input.h"
#include "Engine/Rendering/2D/renderer_2d.h"
#include "Engine/Rendering/2D/sprite_helper.h"
#include "Engine/Systems/animation_system.h"

#if USE_IMGUI
#include "imgui.h"
#endif

namespace YourProject {
  Scene2D::Scene2D() : Scene2D(Engine::Camera2D()) {
  }

  Scene2D::Scene2D(const Engine::Camera2D &camera) : m_camera(camera) {
  }

  Scene2D::~Scene2D() = default;

  void Scene2D::Update(float dt) {
    Engine::AnimationSystem::Update(&m_registry, dt);
    Engine::AudioSystem::Update(&m_registry, dt);
  }

  void Scene2D::Render() {
    auto &renderer = Engine::App::GetLayer<Engine::Renderer2D>();

    // Submit regular sprites
    renderer.Submit(PrepareSprites());

#if USE_IMGUI
    ImGui::ShowDemoWindow();
#endif

    // Submit debug primitives
    // Engine::Shape2DBatch shapeBatch(m_camera.GetViewMatrix());
    // shapeBatch.DrawLine(96.0f, 96.f, 256.0f, 64.0f, Engine::Color{0, 255, 0,
    // 255}, 3.0f); shapeBatch.DrawRectOutlined(64.0f, 64.0f, 32.0f, 64.0f,
    // Engine::Color{255, 0, 0, 255}, 2.0f);
    // shapeBatch.DrawRect(128.0f, 64.0f, 64.0f, 32.0f, Engine::Color{0, 0, 255,
    // 128}, 0.5f); renderer.Submit(shapeBatch);

    renderer.Present();
  }

  void Scene2D::OnEvent(Engine::AppEvent event) {
  }

  void Scene2D::OnDestroy() {
  }

  Engine::Camera2D &Scene2D::GetCamera() { return m_camera; }

  std::vector<Engine::Entity> Scene2D::GetVisibleEntities() {
    return GetVisibleEntities(Engine::Rectf::ZERO);
  }


  std::vector<Engine::Entity> Scene2D::GetVisibleEntities(Engine::Rectf padding) {
    std::vector<entt::entity> ents;
    SDL_FRect result;

    auto camWorldRegion = GetCamera().GetCameraWorldBounds();
    auto camToWorld =
        SDL_FRect{
          camWorldRegion.x - padding.x, camWorldRegion.y - padding.y,
          camWorldRegion.w + padding.w, camWorldRegion.h + padding.h
        };

    // Ideally, we'd want only to retrieve objects that are in the region of the
    // camera rather than iterating over every entity. A good case would be to use
    // a QuadTree or a hybrid approach.
    m_registry.view<Engine::Position, Engine::Size, Engine::Sprite>().each(
      [&](Engine::Entity entity, Engine::Position pos, Engine::Size size,
          Engine::Sprite _) {
        SDL_FRect rect{
          pos.x, pos.y, static_cast<float>(size.x),
          static_cast<float>(size.y)
        };
        if (SDL_GetRectIntersectionFloat(&rect, &camToWorld, &result)) {
          ents.push_back(entity);
        }
      });

    return ents;
  }

  Engine::SpriteSubmission Scene2D::PrepareSprites() {
    return PrepareSprites(m_camera);
  }

  Engine::SpriteSubmission Scene2D::PrepareSprites(Engine::Camera2D &camera) {
    return Engine::GetSpriteSubmission(&m_registry, camera, GetVisibleEntities());
  }
}
