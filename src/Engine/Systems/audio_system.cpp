#include "Engine/Systems/audio_system.h"
#include "Engine/Core/Audio/audio_components.h"
#include "Engine/Core/app.h"
#include "Engine/Core/components.h"
#include "Engine/Layers/audio_manager.h"

namespace Engine {
  void AudioSystem::Update(Registry *registry, float dt) {
    if (!registry)
      return;

    auto &am = App::GetLayer<AudioManager>();
    auto view = registry->view<AudioSource>();
    for (auto entity: view) {
      auto &source = view.get<AudioSource>(entity);

      // Check if track finished playing
      if (source.trackId != INVALID_TRACK_ID &&
          !am.IsTrackPlaying(source.trackId) &&
          !am.IsTrackPaused(source.trackId)) {
        // Track finished (or was stopped manually), cleanup track assignment
        am.DestroyTrack(source.trackId);
        source.trackId = INVALID_TRACK_ID;
      }

      // Start playing if requested
      if (source.autoPlay && source.trackId == INVALID_TRACK_ID &&
          source.audioId != INVALID_AUDIO_ID) {
        source.trackId = am.CreateTrack();
        if (source.trackId != INVALID_TRACK_ID) {
          am.SetTrackAudio(source.trackId, source.audioId);

          PlayOptions options = source.options;
          options.gain = source.gain;

          if (am.PlayTrack(source.trackId, options)) {
            // Success, mark autoPlay as false so we don't start it again
            // continuously
            source.autoPlay = false;
          } else {
            // Failed to play
            am.DestroyTrack(source.trackId);
            source.trackId = INVALID_TRACK_ID;
          }
        }
      }

      // Update 3D position if spatialized
      if (source.spatialize && source.trackId != INVALID_TRACK_ID &&
          am.IsTrackPlaying(source.trackId)) {
        if (registry->all_of<Position>(entity)) {
          const auto &pos = registry->get<Position>(entity);
          AudioPosition3D audioPos(pos);
          am.SetTrack3DPosition(source.trackId, audioPos);
        }
      }
    }
  }
} // namespace Engine
