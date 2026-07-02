#ifndef ENGINE_ANIMATION_SYSTEM_H
#define ENGINE_ANIMATION_SYSTEM_H

#include "Engine/Core/scene_interface.h"

namespace Engine {

    class AnimationSystem {
    public:
        /// This system processes all entities that have both a SpriteAnimator and a Sprite component. 
        /// It advances the current frame of the active animation based on the elapsed delta time, and then 
        /// updates the underlying Sprite's texture ID so it reflects the correct frame. It requires a pointer 
        /// to the ECS registry containing the entities, along with the elapsed time since the last update.
        static void Update(Registry *registry, float dt);
    };

}

#endif // ENGINE_ANIMATION_SYSTEM_H
