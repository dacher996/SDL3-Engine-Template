#include "Engine/Rendering/2D/sprite_helper.h"
#include "Engine/Core/app.h"
#include "Engine/Layers/texture_region_manager.h"
#include "Engine/Layers/material_manager.h"
#include "Engine/Core/logger.h"
#include <format>

namespace Engine {
    void PopulateSpriteEntry(
        SpriteEntry &entry,
        const Sprite &sprite,
        const Position &pos,
        const Size &size,
        const TextureRegion &texRegion) {
        entry.data.x = pos.x;
        entry.data.y = pos.y;
        entry.data.w = static_cast<float>(size.x);
        entry.data.h = static_cast<float>(size.y);

        entry.data.r = static_cast<float>(sprite.color.r) / 255.0f;
        entry.data.g = static_cast<float>(sprite.color.g) / 255.0f;
        entry.data.b = static_cast<float>(sprite.color.b) / 255.0f;
        entry.data.a = static_cast<float>(sprite.color.a) / 255.0f;

        entry.data.rotation = sprite.rotation;

        entry.data.origin_x = sprite.origin.x;
        entry.data.origin_y = sprite.origin.y;

        entry.data.textureLayerId = static_cast<float>(texRegion.layerId);

        entry.data.tex_x = texRegion.x;
        entry.data.tex_y = texRegion.y;
        entry.data.tex_w = texRegion.w;
        entry.data.tex_h = texRegion.h;

        if (sprite.flipSprite == SpriteFlipMode::FLIP_HORIZONTALLY || sprite.flipSprite == SpriteFlipMode::FLIP_BOTH) {
            entry.data.tex_x += texRegion.w;
            entry.data.tex_w = -texRegion.w;
        }

        if (sprite.flipSprite == SpriteFlipMode::FLIP_VERTICALLY || sprite.flipSprite == SpriteFlipMode::FLIP_BOTH) {
            entry.data.tex_y += texRegion.h;
            entry.data.tex_h = -texRegion.h;
        }

        entry.depth = static_cast<Uint16>(pos.z);
    }

    SpriteSubmission GetSpriteSubmission(
        Registry *registry,
        Camera2D &camera,
        const std::vector<Entity> &visibleEntities) {
        SpriteSubmission submission;
        if (!registry) return submission;

        submission.entries.reserve(
            static_cast<size_t>(static_cast<float>(visibleEntities.size()) * 1.2f));
        submission.viewMatrix = camera.GetViewMatrix();

        auto &regionManager = App::GetLayer<TextureRegionManager>();
        auto defaultMaterial = App::GetLayer<MaterialManager>().GetDefaultMaterial();

        for (auto entityId: visibleEntities) {
            const auto &pos = registry->get<Position>(entityId);
            const auto &size = registry->get<Size>(entityId);
            const auto &sprite = registry->get<Sprite>(entityId);
            auto texRegion = regionManager.GetRegion(sprite.spriteId);
            if (!texRegion) {
                ENGINE_LOG_WARN("No texture region with key '{0}' found.", sprite.spriteId);
                continue;
            }

            SpriteEntry entry{};
            PopulateSpriteEntry(entry, sprite, pos, size, *texRegion);

            // Entities with material components produce one entry per material
            if (const auto *materialComp = registry->try_get<MaterialComponent>(entityId);
                materialComp && !materialComp->materialIds.empty()) {
                for (auto mid: materialComp->materialIds) {
                    entry.materialId = mid;
                    submission.entries.push_back(entry);
                }
            } else {
                // If no material component attached, use the default material
                entry.materialId = defaultMaterial->id;
                submission.entries.push_back(entry);
            }
        }

        return submission;
    }
}
