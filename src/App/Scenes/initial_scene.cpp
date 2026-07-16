#include "App/Scenes/initial_scene.h"

#include "App/Core/Parsers/inhouse_spritesheet_parser.h"
#include "App/Scenes/scene_2d.h"
#include "Engine/Core/app.h"
#include "Engine/Core/app_context.h"
#include "Engine/Core/common_identifiers.h"
#include "Engine/Core/logger.h"
#include "Engine/Layers/graphics_pipeline_manager.h"
#include "Engine/Layers/material_manager.h"
#include "Engine/Layers/scene_manager.h"
#include "Engine/Layers/texture_manager.h"
#include "Engine/Layers/texture_region_manager.h"
#include "Engine/Layers/texture_sampler_manager.h"
#include "Engine/Rendering/graphics_pipeline_creation_info.h"

using namespace Engine;

namespace YourProject {
    void InitialScene::Update(float dt) {
        if (m_ranOnce)
            return;
        m_ranOnce = true;

        // Populate level store or other layers as needed

        App::GetLayer<TextureSamplerManager>().GetSampler(POINT_CLAMP);
        if (!LoadShaders())
            return;

        if (!LoadAtlases())
            return;

        SetDefaults();

        // Request window focus
        SDL_RaiseWindow(App::GetLayer<AppContext>().window);

        // Go to next scene after everything has finished loading
        App::GetLayer<SceneManager>().ReplaceScene(new Scene2D());
    }

    void InitialScene::Render() {
    }

    void InitialScene::OnEvent(AppEvent &event) {
    }

    void InitialScene::OnDestroy() {
    }

    void InitialScene::SetDefaults() {
        /// Sets the default texture (which shouldn't be unloaded during app lifecycle)
        App::GetLayer<TextureManager>().SetDefaultTexture(m_defaultTextureId);

        // Set the id of the pixel region for primitive drawing
        App::GetLayer<TextureRegionManager>().SetPixelRegion(m_defaultPixelRegion);

        /// Sets the default (fallback) pipeline pair, used to auto-select between single-texture and
        /// texture-array sampling depending on what a material's bound texture actually is
        App::GetLayer<GraphicsPipelineManager>().SetDefaultPipeline(
            DEFAULT_PIPELINE_ID, DEFAULT_MULTILAYERED_PIPELINE_ID);

        // Create the default material which uses the default pipeline and the main atlas
        App::GetLayer<MaterialManager>().SetDefaultMaterial(
            App::GetLayer<MaterialManager>().CreateMaterial(DEFAULT_PIPELINE_ID, m_defaultTextureId)
        );

        // Upfront create the material for the multilayered pipeline as well
        App::GetLayer<MaterialManager>().CreateMaterial(DEFAULT_MULTILAYERED_PIPELINE_ID, m_defaultTextureId);
    }

    bool InitialScene::LoadShaders() {
        // Load necessary shaders
        auto vertShader{
            GraphicsPipelineManager::LoadShader("Default.vert", 0, 1, 2, 0)
        };
        auto fragShader{
            GraphicsPipelineManager::LoadShader("Default.frag", 1, 0, 0, 0)
        };
        auto multilayeredFragShader{
            GraphicsPipelineManager::LoadShader("DefaultLayeredTexture.frag", 1, 0, 0, 0)
        };
        auto screenVertShader{
            GraphicsPipelineManager::LoadShader("TextureToScreen.vert", 0, 1, 0, 0)
        };
        auto screenFragShader{
            GraphicsPipelineManager::LoadShader("TextureToScreen.frag", 1, 0, 0, 0)
        };

        if (!vertShader || !fragShader || !multilayeredFragShader || !screenVertShader || !screenFragShader) {
            ENGINE_LOG_SDL_ERROR("Unable to load default shaders.");
            return false;
        }

        auto defaultPipeline = GraphicsPipelineManager::CreateGraphicsPipeline(
            vertShader, fragShader, GraphicsPipelineCreationInfo{});
        if (!defaultPipeline) {
            ENGINE_LOG_SDL_ERROR("Unable to create graphics pipeline: default.");
            return false;
        }

        auto defaultMultilayeredPipeline = GraphicsPipelineManager::CreateGraphicsPipeline(
            vertShader, multilayeredFragShader, GraphicsPipelineCreationInfo{});
        if (!defaultMultilayeredPipeline) {
            ENGINE_LOG_SDL_ERROR("Unable to create graphics pipeline: default multilayered.");
            return false;
        }

        auto textureToScreenPipeline =
                GraphicsPipelineManager::CreateGraphicsPipeline(
                    screenVertShader, screenFragShader, GraphicsPipelineCreationInfo{});
        if (!textureToScreenPipeline) {
            ENGINE_LOG_SDL_ERROR(
                "Unable to create graphics pipeline: textureToScreen.");
            return false;
        }

        // Register pipelines
        auto &pipelineManager = App::GetLayer<GraphicsPipelineManager>();
        pipelineManager.RegisterPipeline(DEFAULT_PIPELINE_ID, defaultPipeline);
        pipelineManager.RegisterPipeline(DEFAULT_MULTILAYERED_PIPELINE_ID, defaultMultilayeredPipeline);
        pipelineManager.RegisterPipeline(SCALE_TEXTURE_TO_SCREEN_PIPELINE_ID, textureToScreenPipeline);

        // Free up unused shaders
        GraphicsPipelineManager::UnloadShader(vertShader);
        GraphicsPipelineManager::UnloadShader(fragShader);
        GraphicsPipelineManager::UnloadShader(multilayeredFragShader);
        GraphicsPipelineManager::UnloadShader(screenVertShader);
        GraphicsPipelineManager::UnloadShader(screenFragShader);

        return true;
    }

    bool InitialScene::LoadAtlases() {
        // Load necessary data or assets here
        auto &textureManager = App::GetLayer<TextureManager>();
        std::vector<std::vector<std::string> > atlases{{"atlases/main_atlas.png"}};

        for (auto &atlas: atlases) {
            if (!App::GetLayer<TextureManager>().RegisterTexture(
                m_defaultTextureId, TextureManager::UploadTextures(atlas))) {
                ENGINE_LOG_ERROR("Error uploading texture data to GPU.");
                return false;
            }
        }

        InhouseSpritesheetParser parser("atlases/main_atlas.json", m_defaultTextureId);
        App::GetLayer<TextureRegionManager>().ImportTextureRegions(m_defaultTextureId,
                                                                   parser);

        return true;
    }
}
