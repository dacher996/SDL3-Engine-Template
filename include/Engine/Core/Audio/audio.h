#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

#include <string>
#include <vector>
#include <unordered_map>

#include "Engine/Core/Audio/audio_types.h"

namespace Engine {
    class Audio {
    public:
        /// Play a sound effect from a file path. If the file hasn't been loaded
        /// yet, it will be loaded automatically and cached for future calls.
        /// SFX are pre-decoded by default since they tend to be short and
        /// frequently played. Returns the TrackId of the track used for
        /// playback, which can be used to control the sound after it starts.
        static TrackId PlaySFX(const char *path, float gain = 1.0f);

        /// Play a sound effect from a file path with full control over playback
        /// options. Auto-loads and caches the file if needed.
        static TrackId PlaySFX(const char *path, const PlayOptions &options);

        /// Play a previously loaded audio asset as a sound effect. This skips
        /// auto-loading entirely, so the caller is responsible for ensuring the
        /// AudioId is valid.
        static TrackId PlaySFX(AudioId audioId, float gain = 1.0f);

        /// Play a previously loaded audio asset as a sound effect with full
        /// control over playback options.
        static TrackId PlaySFX(AudioId audioId, const PlayOptions &options);

        /// Play a sound effect positioned in 3D space. The sound will be
        /// spatialized based on its distance and direction from the listener
        /// at the origin. Auto-loads the file if needed.
        static TrackId PlaySFXAt(const char *path, const AudioPosition3D &position, float gain = 1.0f);

        /// Play a previously loaded audio asset positioned in 3D space.
        static TrackId PlaySFXAt(AudioId audioId, const AudioPosition3D &position, float gain = 1.0f);

        /// Play background music from a file path. Music loops by default, and
        /// only one music track can be active at a time — calling this while
        /// music is already playing will stop the previous track first. Music is
        /// not pre-decoded by default since music files tend to be large and
        /// streaming from compressed data saves significant memory.
        static void PlayMusic(const char *path, float gain = 1.0f);

        /// Play background music from a file path with full control over
        /// playback options. Stops any currently playing music first.
        static void PlayMusic(const char *path, const PlayOptions &options);

        /// Play a previously loaded audio asset as background music.
        /// Stops any currently playing music first.
        static void PlayMusic(AudioId audioId, float gain = 1.0f);

        /// Play a previously loaded audio asset as background music with full
        /// control over playback options.
        static void PlayMusic(AudioId audioId, const PlayOptions &options);

        /// Stop the current music immediately.
        static void StopMusic();

        /// Stop the current music with a gradual fade-out over the given
        /// number of milliseconds.
        static void StopMusic(Uint32 fadeOutMs);

        /// Pause the current music, retaining its playback position.
        static void PauseMusic();

        /// Resume previously paused music from where it left off.
        static void ResumeMusic();

        /// Returns true if music is currently playing (not paused, not stopped).
        static bool IsMusicPlaying();

        /// Set the master volume for all audio output. This affects everything —
        /// music, SFX, and any other tracks. Range is 0.0 (silent) to 1.0 (full).
        static void SetMasterVolume(float volume);

        /// Set the volume for all SFX tracks. This uses the SFX tag to find
        /// and adjust all tracks in the SFX pool.
        static void SetSFXVolume(float volume);

        /// Set the volume for the music track.
        static void SetMusicVolume(float volume);

        /// Pause all audio output — both music and SFX.
        static void PauseAll();

        /// Resume all paused audio.
        static void ResumeAll();

        /// Stop all audio output.
        static void StopAll();

    private:
        /// Make sure an audio file is loaded, loading it if this is the first
        /// time we've seen this path. Returns the AudioId for the loaded asset.
        static AudioId EnsureLoaded(const char *path, bool predecode);

        /// Get a free SFX track from the pool. If all tracks in the pool are
        /// currently playing, a new track is created and added to the pool.
        static TrackId AcquireSFXTrack();

        static TrackId s_musicTrackId;
        static std::unordered_map<std::string, AudioId> s_audioCache;

        /// Pool of reusable SFX tracks. Pre-allocated to
        /// DEFAULT_SFX_TRACK_POOL_SIZE on first use, grows on demand.
        static std::vector<TrackId> s_sfxTrackPool;
    };
}

#endif // ENGINE_AUDIO_H
