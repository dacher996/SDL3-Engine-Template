#ifndef ENGINE_AUDIO_SYSTEM_H
#define ENGINE_AUDIO_SYSTEM_H

#include "Engine/Core/scene_interface.h"

namespace Engine {
    class AudioSystem {
    public:
        /// Process all entities that have an AudioSource component. This does
        /// three things each frame:
        ///
        /// First, for any AudioSource that has autoPlay set and hasn't been
        /// started yet (trackId is INVALID_TRACK_ID), it creates a track via
        /// the AudioManager, assigns the audio, and begins playback.
        ///
        /// Second, for any AudioSource where spatialize is true and the entity
        /// also has a Position component, it updates the track's 3D position
        /// to match the entity's current position. This makes sounds follow
        /// their entities as they move around the world.
        ///
        /// Third, for any AudioSource whose track has finished playing (not
        /// looping), it resets the trackId back to INVALID_TRACK_ID so the
        /// track can be cleaned up or reused.
        static void Update(Registry *registry, float dt);
    };
}

#endif // ENGINE_AUDIO_SYSTEM_H
