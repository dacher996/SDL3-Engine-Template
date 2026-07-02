#include "Engine/Systems/animation_system.h"
#include "Engine/Core/components.h"

namespace Engine {
  void AnimationSystem::Update(Registry *registry, float dt) {
    if (!registry)
      return;
    auto view = registry->view<SpriteAnimator, Sprite>();

    for (auto entity: view) {
      auto &animator = view.get<SpriteAnimator>(entity);
      auto &sprite = view.get<Sprite>(entity);

      if (!animator.isPlaying || animator.animations.empty()) {
        continue;
      }

      auto it = animator.animations.find(animator.currentAnimation);
      if (it == animator.animations.end()) {
        continue;
      }

      const auto &clip = it->second;
      if (clip.frames.empty() || clip.framesPerSecond <= 0.0f) {
        continue;
      }

      animator.currentTime += dt * animator.speedMultiplier;
      float durationPerFrame = 1.0f / clip.framesPerSecond;
      int framesToAdvance =
          static_cast<int>(animator.currentTime / durationPerFrame);

      if (framesToAdvance > 0) {
        animator.currentTime -= framesToAdvance * durationPerFrame;
        int totalFrames = static_cast<int>(clip.frames.size());

        switch (clip.mode) {
          case AnimationMode::OneShot: {
            animator.currentFrameIndex += framesToAdvance;
            if (animator.currentFrameIndex >= totalFrames) {
              animator.currentFrameIndex = totalFrames - 1;
              animator.isPlaying = false;
            }
            break;
          }
          case AnimationMode::Loop: {
            animator.currentFrameIndex =
                (animator.currentFrameIndex + framesToAdvance) % totalFrames;
            break;
          }
          case AnimationMode::PingPong: {
            for (int i = 0; i < framesToAdvance; ++i) {
              if (animator.isReversing) {
                animator.currentFrameIndex--;
                if (animator.currentFrameIndex < 0) {
                  animator.currentFrameIndex = 1; // bounce back
                  animator.isReversing = false;
                }
              } else {
                animator.currentFrameIndex++;
                if (animator.currentFrameIndex >= totalFrames) {
                  animator.currentFrameIndex = totalFrames - 2; // bounce back
                  animator.isReversing = true;
                  // if only 1 frame, clamp it
                  if (animator.currentFrameIndex < 0)
                    animator.currentFrameIndex = 0;
                }
              }
            }
            break;
          }
          case AnimationMode::ReverseOneShot: {
            animator.currentFrameIndex -= framesToAdvance;
            if (animator.currentFrameIndex < 0) {
              animator.currentFrameIndex = 0;
              animator.isPlaying = false;
            }
            break;
          }
          case AnimationMode::ReverseLoop: {
            animator.currentFrameIndex -= framesToAdvance % totalFrames;
            if (animator.currentFrameIndex < 0) {
              animator.currentFrameIndex += totalFrames;
            }
            break;
          }
        }
      }

      sprite.spriteId = clip.frames[animator.currentFrameIndex];
    }
  }
} // namespace Engine
