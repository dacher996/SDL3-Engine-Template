#ifndef APP_ENGINE_ENGINE_H
#define APP_ENGINE_ENGINE_H

#include "Core/Input/input.h"
#include "Core/app.h"
#include "Core/app_context.h"
#include "Core/utils.h"

#include "Layers/graphics_pipeline_manager.h"
#include "Layers/imgui_manager.h"
#include "Layers/input_manager.h"
#include "Layers/material_manager.h"
#include "Layers/scene_manager.h"
#include "Layers/texture_manager.h"
#include "Layers/texture_region_manager.h"
#include "Layers/texture_sampler_manager.h"

#include "Rendering/2D/default_2d_render_pass.h"
#include "Rendering/2D/gpu_sprite.h"
#include "Rendering/2D/renderer_2d.h"
#include "Rendering/2D/scene_models_2d.h"
#include "Rendering/2D/shape_2d_batch.h"
#include "Rendering/2D/sprite_helper.h"
#include "Rendering/RenderPasses/imgui_render_pass.h"
#include "Rendering/RenderPasses/render_target_render_pass.h"

#include "Systems/animation_system.h"

#if USE_IMGUI
#include "imgui.h"
#endif

#endif // APP_ENGINE_ENGINE_H
