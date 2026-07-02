#ifndef ENGINE_SPRITE_HELPER_H
#define ENGINE_SPRITE_HELPER_H

#include "Engine/Rendering/sprite_submission.h"
#include "Engine/Core/components.h"
#include "Engine/Core/texture_region.h"
#include "Engine/Core/scene_interface.h"
#include "Engine/Rendering/2D/camera_2d.h"

namespace Engine {

    /// Helper to cleanly populate a GPUSprite entry from high-level components.
    /// Handles applying origin, offset flipping UVs, and mapping component data automatically.
    void PopulateSpriteEntry(
            SpriteEntry &entry,
            const Sprite &sprite,
            const Position &pos,
            const Size &size,
            const TextureRegion &texRegion);

    /// Helper to generate a complete SpriteSubmission from a list of visible entities.
    SpriteSubmission GetSpriteSubmission(
            Registry *registry,
            Camera2D &camera,
            const std::vector<Entity> &visibleEntities);

}

#endif // ENGINE_SPRITE_HELPER_H
