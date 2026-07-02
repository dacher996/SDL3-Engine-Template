#ifndef ENGINE_COMPONENTS_H
#define ENGINE_COMPONENTS_H

#include "datatypes.h"

#include <vector>
#include <unordered_map>
#include "color.h"

namespace Engine {
    using Size = Vec2i;
    using Position = Vec3f;

    struct MaterialComponent {
        std::vector<Uint16> materialIds;
    };

    enum class SpriteFlipMode {
        FLIP_NONE,
        FLIP_HORIZONTALLY,
        FLIP_VERTICALLY,
        FLIP_BOTH
    };

    struct Sprite {
        Uint32 spriteId;
        float rotation{0};
        Color color{Color::WHITE};
        Vec2f origin{0.5f, 0.5f};
        SpriteFlipMode flipSprite{SpriteFlipMode::FLIP_NONE};
    };

    enum class AnimationMode {
        OneShot,
        Loop,
        PingPong,
        ReverseOneShot,
        ReverseLoop
    };

    struct AnimationClip {
        std::vector<Uint32> frames;
        float framesPerSecond{12.0f};
        AnimationMode mode{AnimationMode::Loop};
    };

    struct SpriteAnimator {
        std::unordered_map<Uint32, AnimationClip> animations;
        Uint32 currentAnimation{0};
        
        float currentTime{0.0f};
        bool isPlaying{false};
        int currentFrameIndex{0};
        bool isReversing{false};
        float speedMultiplier{1.0f};

        void Play(Uint32 animationId) {
            currentAnimation = animationId;
            currentTime = 0.0f;
            currentFrameIndex = 0;
            isPlaying = true;
            isReversing = false;
        }

        void Stop() {
            isPlaying = false;
            currentTime = 0.0f;
            currentFrameIndex = 0;
            isReversing = false;
        }

        void Pause() {
            isPlaying = false;
        }

        void Resume() {
            isPlaying = true;
        }
    };
}

#endif //ENGINE_COMPONENTS_H
