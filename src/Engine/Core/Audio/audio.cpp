#include "Engine/Core/Audio/audio.h"
#include "Engine/Core/app.h"
#include "Engine/Layers/audio_manager.h"

namespace Engine {
    TrackId Audio::s_musicTrackId = INVALID_TRACK_ID;
    std::unordered_map<std::string, AudioId> Audio::s_audioCache;
    std::vector<TrackId> Audio::s_sfxTrackPool;

    constexpr AudioTag TAG_SFX = 1;
    constexpr AudioTag TAG_MUSIC = 2;

    AudioId Audio::EnsureLoaded(const char *path, bool predecode) {
        auto it = s_audioCache.find(path);
        if (it != s_audioCache.end()) {
            return it->second;
        }

        auto &am = App::GetLayer<AudioManager>();
        AudioId id = am.LoadAudio(path, predecode);
        if (id != INVALID_AUDIO_ID) {
            s_audioCache[path] = id;
        }
        return id;
    }

    TrackId Audio::AcquireSFXTrack() {
        auto &am = App::GetLayer<AudioManager>();

        if (s_sfxTrackPool.empty()) {
            for (int i = 0; i < DEFAULT_SFX_TRACK_POOL_SIZE; ++i) {
                TrackId id = am.CreateTrack();
                if (id != INVALID_TRACK_ID) {
                    am.TagTrack(id, TAG_SFX);
                    s_sfxTrackPool.push_back(id);
                }
            }
        }

        for (TrackId id: s_sfxTrackPool) {
            if (!am.IsTrackPlaying(id)) {
                return id;
            }
        }

        TrackId id = am.CreateTrack();
        if (id != INVALID_TRACK_ID) {
            am.TagTrack(id, TAG_SFX);
            s_sfxTrackPool.push_back(id);
        }
        return id;
    }

    TrackId Audio::PlaySFX(const char *path, float gain) {
        PlayOptions options;
        options.gain = gain;
        options.predecode = true; // SFX default
        return PlaySFX(path, options);
    }

    TrackId Audio::PlaySFX(const char *path, const PlayOptions &options) {
        AudioId audioId = EnsureLoaded(path, options.predecode);
        if (audioId == INVALID_AUDIO_ID) return INVALID_TRACK_ID;
        return PlaySFX(audioId, options);
    }

    TrackId Audio::PlaySFX(AudioId audioId, float gain) {
        PlayOptions options;
        options.gain = gain;
        return PlaySFX(audioId, options);
    }

    TrackId Audio::PlaySFX(AudioId audioId, const PlayOptions &options) {
        TrackId trackId = AcquireSFXTrack();
        if (trackId == INVALID_TRACK_ID) return INVALID_TRACK_ID;

        auto &am = App::GetLayer<AudioManager>();
        am.SetTrackAudio(trackId, audioId);

        PlayOptions finalOptions = options;
        if (finalOptions.tag == NO_TAG) {
            finalOptions.tag = TAG_SFX; // Ensure it retains SFX tag if none provided
        }

        if (am.PlayTrack(trackId, finalOptions)) {
            return trackId;
        }
        return INVALID_TRACK_ID;
    }

    TrackId Audio::PlaySFXAt(const char *path, const AudioPosition3D &position, float gain) {
        PlayOptions options;
        options.gain = gain;
        options.predecode = true;

        AudioId audioId = EnsureLoaded(path, options.predecode);
        if (audioId == INVALID_AUDIO_ID) return INVALID_TRACK_ID;
        return PlaySFXAt(audioId, position, gain);
    }

    TrackId Audio::PlaySFXAt(AudioId audioId, const AudioPosition3D &position, float gain) {
        PlayOptions options;
        options.gain = gain;

        TrackId trackId = AcquireSFXTrack();
        if (trackId == INVALID_TRACK_ID) return INVALID_TRACK_ID;

        auto &am = App::GetLayer<AudioManager>();
        am.SetTrackAudio(trackId, audioId);
        am.SetTrack3DPosition(trackId, position);

        options.tag = TAG_SFX;

        if (am.PlayTrack(trackId, options)) {
            return trackId;
        }
        return INVALID_TRACK_ID;
    }

    void Audio::PlayMusic(const char *path, float gain) {
        PlayOptions options;
        options.gain = gain;
        options.loop = LoopMode::Loop;
        options.predecode = false; // Music default
        PlayMusic(path, options);
    }

    void Audio::PlayMusic(const char *path, const PlayOptions &options) {
        AudioId audioId = EnsureLoaded(path, options.predecode);
        if (audioId == INVALID_AUDIO_ID) return;
        PlayMusic(audioId, options);
    }

    void Audio::PlayMusic(AudioId audioId, float gain) {
        PlayOptions options;
        options.gain = gain;
        options.loop = LoopMode::Loop;
        PlayMusic(audioId, options);
    }

    void Audio::PlayMusic(AudioId audioId, const PlayOptions &options) {
        auto &am = App::GetLayer<AudioManager>();

        if (s_musicTrackId == INVALID_TRACK_ID) {
            s_musicTrackId = am.CreateTrack();
            if (s_musicTrackId != INVALID_TRACK_ID) {
                am.TagTrack(s_musicTrackId, TAG_MUSIC);
            }
        }

        if (s_musicTrackId == INVALID_TRACK_ID) return;

        am.StopTrack(s_musicTrackId);
        am.SetTrackAudio(s_musicTrackId, audioId);

        PlayOptions finalOptions = options;
        if (finalOptions.tag == NO_TAG) {
            finalOptions.tag = TAG_MUSIC;
        }

        am.PlayTrack(s_musicTrackId, finalOptions);
    }

    void Audio::StopMusic() {
        if (s_musicTrackId != INVALID_TRACK_ID) {
            App::GetLayer<AudioManager>().StopTrack(s_musicTrackId);
        }
    }

    void Audio::StopMusic(Uint32 fadeOutMs) {
        if (s_musicTrackId != INVALID_TRACK_ID) {
            App::GetLayer<AudioManager>().StopTrack(s_musicTrackId, fadeOutMs);
        }
    }

    void Audio::PauseMusic() {
        if (s_musicTrackId != INVALID_TRACK_ID) {
            App::GetLayer<AudioManager>().PauseTrack(s_musicTrackId);
        }
    }

    void Audio::ResumeMusic() {
        if (s_musicTrackId != INVALID_TRACK_ID) {
            App::GetLayer<AudioManager>().ResumeTrack(s_musicTrackId);
        }
    }

    bool Audio::IsMusicPlaying() {
        if (s_musicTrackId != INVALID_TRACK_ID) {
            return App::GetLayer<AudioManager>().IsTrackPlaying(s_musicTrackId);
        }
        return false;
    }

    void Audio::SetMasterVolume(float volume) {
        App::GetLayer<AudioManager>().SetMasterGain(volume);
    }

    void Audio::SetSFXVolume(float volume) {
        App::GetLayer<AudioManager>().SetGainByTag(TAG_SFX, volume);
    }

    void Audio::SetMusicVolume(float volume) {
        App::GetLayer<AudioManager>().SetGainByTag(TAG_MUSIC, volume);
    }

    void Audio::PauseAll() {
        App::GetLayer<AudioManager>().PauseAll();
    }

    void Audio::ResumeAll() {
        App::GetLayer<AudioManager>().ResumeAll();
    }

    void Audio::StopAll() {
        App::GetLayer<AudioManager>().StopAll();
    }
}
