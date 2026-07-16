#ifndef ENGINE_COMMON_IDENTIFIERS_H
#define ENGINE_COMMON_IDENTIFIERS_H

#include <SDL3/SDL_stdinc.h>

namespace Engine {
    /// Default pipeline id for rendering of a single texture
    constexpr Uint16 DEFAULT_PIPELINE_ID = 0xffff;

    /// Default pipeline id for rendering of a multi-layered texture
    constexpr Uint16 DEFAULT_MULTILAYERED_PIPELINE_ID = 0xfffe;

    /// Pipeline id used for rendering a scaled texture to screen
    constexpr Uint16 SCALE_TEXTURE_TO_SCREEN_PIPELINE_ID = 0xfffd;
}

#endif //ENGINE_COMMON_IDENTIFIERS_H
