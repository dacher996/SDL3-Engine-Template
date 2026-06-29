#ifndef ENGINE_SHAPE_2D_BATCH_H
#define ENGINE_SHAPE_2D_BATCH_H

#include "Engine/Core/color.h"
#include "Engine/Rendering/sprite_submission.h"
#include <glm/glm.hpp>

#include "Engine/Core/datatypes.h"
#include "Engine/Core/texture_region.h"

#define SHAPE_2D_BATCH_MAX_DEPTH 65535

namespace Engine {
    /// Helper builder to construct primitive drawing submissions
    class Shape2DBatch {
    public:
        /// Creates a batch using the provided camera view matrix.
        explicit Shape2DBatch(const glm::mat4 &viewMatrix);

        /// Draws a line from (x0, y0) to (x1, y1) with the given color, thickness, and
        /// depth (z-index).
        void DrawLine(float x0, float y0, float x1, float y1, const Color &color,
                      float thickness = 1.0f,
                      Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Draws a line from pt0 to pt1 with the given color, thickness, and depth
        /// (z-index).
        void DrawLine(Vec2f pt0, Vec2f pt1, const Color &color,
                      float thickness = 1.0f,
                      Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Draws a filled rectangle at (x, y) with the given width and height.
        /// The rotation is in radians. You can also specify the color and depth
        /// (z-index).
        void DrawRect(float x, float y, float w, float h, const Color &color,
                      float rotation = 0.0f, Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Draws a filled rectangle using a Rectf object for its bounds.
        /// The rotation is in radians. You can also specify the color and depth
        /// (z-index).
        void DrawRect(Rectf rect, const Color &color, float rotation = 0.0f,
                      Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Draws the outline of a rectangle at (x, y) with the given width, height,
        /// and line thickness. The rotation is in radians. You can also specify the
        /// color and depth (z-index).
        void DrawRectOutlined(float x, float y, float w, float h, const Color &color,
                              float thickness = 1.0f, float rotation = 0.0f,
                              Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Draws the outline of a rectangle using a Rectf object for its bounds, with
        /// the given line thickness. The rotation is in radians. You can also specify
        /// the color and depth (z-index).
        void DrawRectOutlined(Rectf rect, const Color &color, float thickness = 1.0f,
                              float rotation = 0.0f,
                              Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Draws a texture region at (x, y) with the given width and height.
        /// You can tint it with a color, and the rotation is in radians. You can also
        /// specify the depth (z-index).
        void DrawTexture(float x, float y, float w, float h,
                         const TextureRegion *region,
                         const Color &color = Color::WHITE, float rotation = 0.0f,
                         Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Draws a texture region using a Rectf object for its destination bounds.
        /// You can tint it with a color, and the rotation is in radians. You can also
        /// specify the depth (z-index).
        void DrawTexture(Rectf rect, const TextureRegion *region,
                         const Color &color = Color::WHITE, float rotation = 0.0f,
                         Uint16 depth = SHAPE_2D_BATCH_MAX_DEPTH);

        /// Returns the completed sprite submission
        SpriteSubmission End();

    private:
        SpriteSubmission m_submission;
    };
} // namespace Engine

#endif // ENGINE_SHAPE_2D_BATCH_H
