#ifndef ENGINE_AUDIO_TYPES_H
#define ENGINE_AUDIO_TYPES_H

#include "Engine/Core/datatypes.h"

/// Default number of pre-allocated SFX tracks in the convenience Audio API's pool.
/// Tracks grow on demand beyond this limit. Adjust to match your game's typical
/// simultaneous SFX count.
#define DEFAULT_SFX_TRACK_POOL_SIZE 16

namespace Engine {
    /// Opaque handle to a loaded audio asset (maps to an internal MIX_Audio*)
    using AudioId = Uint32;

    /// Opaque handle to a playback track (maps to an internal MIX_Track*)
    using TrackId = Uint32;

    /// Integer tag for grouping tracks (e.g., for music, sfx, ui categories).
    /// Use application-defined constants or an enum to assign meaning to tag values.
    using AudioTag = int;

    constexpr AudioId INVALID_AUDIO_ID = 0;
    constexpr TrackId INVALID_TRACK_ID = 0;
    constexpr AudioTag NO_TAG = -1;

    /// Looping behavior
    enum class LoopMode {
        Once, // Play once then stop
        Loop, // Loop forever
        Count // Loop N times (use with PlayOptions::loopCount)
    };

    /// Fade direction (read-only query result)
    enum class FadeState {
        None,
        FadingIn,
        FadingOut
    };

    /// Options for playing audio. All fields have sensible defaults so the struct
    /// is usable as-is for the simplest case. Only set the fields you need.
    struct PlayOptions {
        LoopMode loop = LoopMode::Once;
        int loopCount = 0; // Only meaningful when loop is LoopMode::Count

        float gain = 1.0f; // Volume multiplier: 0.0 = silent, 1.0 = default, >1.0 = amplify

        Uint32 fadeInMs = 0; // Fade-in duration in milliseconds (0 = no fade)

        Uint64 startFrame = 0; // Start offset in sample frames
        Uint64 endFrame = 0; // End position in sample frames (0 = play to end)

        AudioTag tag = NO_TAG; // Optional tag for group operations

        /// Whether to pre-decode the audio data on load.
        ///
        /// When set to true, the entire audio file is decompressed into raw PCM
        /// during loading. This uses significantly more memory, but playback
        /// requires zero CPU for decoding. This is best for short, frequently
        /// played sounds (SFX) like footsteps, gunshots, or UI clicks — things
        /// that may play many times per second or even multiple times per frame.
        ///
        /// When set to false (the default), the audio stays compressed in memory
        /// in its original format (MP3, OGG, etc.) and is decoded on-the-fly
        /// during playback. This uses minimal memory but requires some CPU per
        /// playing instance. This is best for long, infrequent audio like
        /// background music or ambient loops.
        bool predecode = false;
    };

    /// 3D position for spatial audio.
    ///
    /// SDL_mixer uses a right-handed coordinate system with the listener fixed at
    /// the origin (0, 0, 0). Distance from the origin controls attenuation, and
    /// the position relative to the listener controls stereo/surround panning.
    struct AudioPosition3D {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        AudioPosition3D() = default;

        /// Construct from explicit coordinates.
        AudioPosition3D(float x, float y, float z) : x(x), y(y), z(z) {
        }

        /// Construct from a 2D position. The z coordinate defaults to 0.
        AudioPosition3D(const Vec2f &v) : x(v.x), y(v.y), z(0.0f) {
        }

        /// Construct from a 3D position.
        AudioPosition3D(const Vec3f &v) : x(v.x), y(v.y), z(v.z) {
        }
    };

    /// Stereo panning control.
    ///
    /// Forces a track's output to the Front Left and Front Right speakers,
    /// letting you pan audio between them. Setting stereo pan on a track clears
    /// any 3D positioning on that track, and vice versa — they are mutually
    /// exclusive spatialization modes.
    struct StereoPan {
        float left = 1.0f; // Gain for left channel, from 0.0 (silent) to 1.0 (full)
        float right = 1.0f; // Gain for right channel, from 0.0 (silent) to 1.0 (full)
    };
}

#endif // ENGINE_AUDIO_TYPES_H
