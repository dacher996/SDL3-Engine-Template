#ifndef ENGINE_AUDIO_COMPONENTS_H
#define ENGINE_AUDIO_COMPONENTS_H

#include "Engine/Core/Audio/audio_types.h"

namespace Engine {
    /// Attach this component to an entity to give it a sound source.
    ///
    /// The AudioSystem processes entities with this component each frame. If
    /// the entity also has a Position component and spatialize is true, the
    /// system will automatically update the track's 3D position to match the
    /// entity's world position, so sounds follow their entities around.
    struct AudioSource {
        /// The audio asset to play.
        AudioId audioId = INVALID_AUDIO_ID;

        /// The track currently playing this source. This is managed by the
        /// AudioSystem — you don't need to set it yourself. It gets assigned
        /// when playback starts and reset to INVALID_TRACK_ID when it stops.
        TrackId trackId = INVALID_TRACK_ID;

        /// Playback options for this source.
        PlayOptions options;

        /// When true, the AudioSystem will automatically start playing this
        /// source the first time it encounters it (i.e., right after the
        /// component is added to an entity).
        bool autoPlay = false;

        /// When true and the entity has a Position component, the AudioSystem
        /// will sync the track's 3D position from the entity's Position each
        /// frame, so the sound follows the entity.
        bool spatialize = true;

        /// Gain (volume) for this source.
        float gain = 1.0f;
    };
}

#endif // ENGINE_AUDIO_COMPONENTS_H
