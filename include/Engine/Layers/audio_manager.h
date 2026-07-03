#ifndef ENGINE_AUDIO_MANAGER_H
#define ENGINE_AUDIO_MANAGER_H

#include <unordered_map>

#include "Engine/Core/Audio/audio_types.h"
#include "Engine/Core/layer_interface.h"

struct MIX_Mixer;
struct MIX_Audio;
struct MIX_Track;

namespace Engine {
    class AudioManager : public ILayer {
    public:
        AudioManager();

        /// Initialize SDL_mixer and create the default mixer device.
        /// Called automatically during App::Init().
        bool Init();

        /// Destroy all tracks, unload all audio, shut down SDL_mixer.
        void Cleanup();

        /// Load an audio file from disk and return a handle to it. If predecode
        /// is true, the entire file is decompressed into raw PCM at load time,
        /// which costs more memory but eliminates decoding overhead during
        /// playback. For short SFX that play frequently, predecoding is usually
        /// the right choice. For long music tracks, leave it false to save memory.
        /// Returns INVALID_AUDIO_ID on failure.
        AudioId LoadAudio(const char *path, bool predecode = false);

        /// Unload a previously loaded audio asset, freeing its memory.
        /// Any tracks still referencing this audio should be stopped first.
        void UnloadAudio(AudioId id);

        /// Check if an audio asset with the given id is currently loaded.
        bool IsAudioLoaded(AudioId id) const;

        /// Create a new playback track on the mixer. Returns its handle, or
        /// INVALID_TRACK_ID on failure. Tracks are lightweight — create as many
        /// as you need for simultaneous sounds.
        TrackId CreateTrack();

        /// Destroy a track and free its resources. If the track is currently
        /// playing, it will be stopped first.
        void DestroyTrack(TrackId id);

        /// Assign a loaded audio asset to a track. The track must exist and the
        /// audio must be loaded. This can be called while the track is stopped or
        /// playing (it will take effect on next play).
        void SetTrackAudio(TrackId trackId, AudioId audioId);

        /// Play a track with default options. The track must have audio assigned
        /// via SetTrackAudio before calling this. Returns true on success.
        bool PlayTrack(TrackId trackId);

        /// Play a track with full control over playback parameters including
        /// looping, gain, fade-in/out, and start/end positions. The options
        /// struct has sensible defaults so you only need to set what you want
        /// to customize. Returns true on success.
        bool PlayTrack(TrackId trackId, const PlayOptions &options);

        /// Stop a track immediately. The track remains valid and can be
        /// replayed later.
        void StopTrack(TrackId trackId);

        /// Stop a track with a gradual fade-out over the given number of
        /// milliseconds. The track will continue playing at decreasing volume
        /// until it reaches silence, then stop automatically.
        void StopTrack(TrackId trackId, Uint32 fadeOutMs);

        /// Pause a track, retaining its current playback position. Call
        /// ResumeTrack to continue from where it left off.
        void PauseTrack(TrackId trackId);

        /// Resume a previously paused track from its saved position.
        /// Has no effect if the track is not paused.
        void ResumeTrack(TrackId trackId);

        /// Returns true if the track is currently producing audio output.
        /// A paused track is not considered playing.
        bool IsTrackPlaying(TrackId trackId) const;

        /// Returns true if the track is currently paused.
        bool IsTrackPaused(TrackId trackId) const;

        /// Set the gain (volume) for a specific track. A gain of 1.0 is the
        /// default volume, 0.0 is completely silent, and values above 1.0
        /// amplify the signal (which may cause clipping).
        void SetTrackGain(TrackId trackId, float gain);

        /// Get the current gain of a track. Returns 1.0 if the track doesn't
        /// exist or has no gain set.
        float GetTrackGain(TrackId trackId) const;

        /// Set the master gain applied to the mixer's final output. This
        /// multiplies on top of all individual track gains. 1.0 is the default.
        void SetMasterGain(float gain);

        /// Get the current master gain.
        float GetMasterGain() const;

        /// Set a track's 3D position for distance attenuation and
        /// spatialization. The listener is fixed at the origin (0, 0, 0).
        /// Sounds further from the origin will be quieter, and their stereo
        /// or surround placement will reflect their position relative to the
        /// listener. Note that setting a 3D position clears any stereo pan
        /// previously set on this track.
        void SetTrack3DPosition(TrackId trackId, const AudioPosition3D &position);

        /// Clear 3D positioning from a track, disabling distance attenuation
        /// and spatialization. The track will play at full volume through all
        /// speakers as if it had no spatial properties.
        void ClearTrack3DPosition(TrackId trackId);

        /// Set stereo panning for a track, forcing its output to the front
        /// left and right speakers only. The left and right gain values
        /// control the balance. Note that setting stereo pan clears any 3D
        /// position previously set on this track.
        void SetTrackStereoPan(TrackId trackId, const StereoPan &pan);

        /// Clear stereo panning from a track, restoring default speaker routing.
        void ClearTrackStereoPan(TrackId trackId);

        /// Tag a track with an integer label for group operations. Tags let
        /// you control multiple tracks at once — for example, pausing all
        /// tracks tagged as "sfx" while leaving "music" running. A track can
        /// only have one tag at a time; setting a new tag replaces the old one.
        void TagTrack(TrackId trackId, AudioTag tag);

        /// Stop all tracks that have the given tag.
        void StopByTag(AudioTag tag);

        /// Stop all tracks with the given tag, fading out over the given
        /// number of milliseconds.
        void StopByTag(AudioTag tag, Uint32 fadeOutMs);

        /// Pause all tracks with the given tag.
        void PauseByTag(AudioTag tag);

        /// Resume all tracks with the given tag.
        void ResumeByTag(AudioTag tag);

        /// Set the gain for all tracks with the given tag.
        void SetGainByTag(AudioTag tag, float gain);

        /// Pause every track on the mixer.
        void PauseAll();

        /// Resume every track on the mixer.
        void ResumeAll();

        /// Stop every track on the mixer.
        void StopAll();

    private:
        MIX_Mixer *m_mixer = nullptr;
        float m_masterGain = 1.0f;

        AudioId m_nextAudioId = 1;
        TrackId m_nextTrackId = 1;

        std::unordered_map<AudioId, MIX_Audio *> m_loadedAudio;
        std::unordered_map<TrackId, MIX_Track *> m_tracks;
        std::unordered_map<TrackId, AudioTag> m_trackTags;
        std::unordered_map<TrackId, float> m_trackGains;
    };
}

#endif // ENGINE_AUDIO_MANAGER_H
