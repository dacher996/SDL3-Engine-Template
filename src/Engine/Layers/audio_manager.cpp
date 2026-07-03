#include "Engine/Layers/audio_manager.h"

#include "Engine/Core/logger.h"

#include "SDL3_mixer/SDL_mixer.h"

namespace Engine {
    AudioManager::AudioManager() {
        Init();
    }

    bool AudioManager::Init() {
        if (!MIX_Init()) {
            ENGINE_LOG_ERROR("SDL_mixer could not initialize! SDL_mixer Error: %s", SDL_GetError());
            return false;
        }

        m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!m_mixer) {
            ENGINE_LOG_ERROR("SDL_mixer device could not be created! SDL_mixer Error: %s", SDL_GetError());
            return false;
        }

        return true;
    }

    void AudioManager::Cleanup() {
        for (auto &pair: m_tracks) {
            MIX_DestroyTrack(pair.second);
        }
        m_tracks.clear();

        for (auto &pair: m_loadedAudio) {
            MIX_DestroyAudio(pair.second);
        }
        m_loadedAudio.clear();

        if (m_mixer) {
            MIX_DestroyMixer(m_mixer);
            m_mixer = nullptr;
        }

        MIX_Quit();
    }

    AudioId AudioManager::LoadAudio(const char *path, bool predecode) {
        if (!m_mixer) return INVALID_AUDIO_ID;

        MIX_Audio *audio = MIX_LoadAudio(m_mixer, path, predecode);
        if (!audio) {
            ENGINE_LOG_ERROR("Failed to load audio '%s'! SDL_mixer Error: %s", path, SDL_GetError());
            return INVALID_AUDIO_ID;
        }

        AudioId id = m_nextAudioId++;
        m_loadedAudio[id] = audio;
        return id;
    }

    void AudioManager::UnloadAudio(AudioId id) {
        auto it = m_loadedAudio.find(id);
        if (it != m_loadedAudio.end()) {
            MIX_DestroyAudio(it->second);
            m_loadedAudio.erase(it);
        }
    }

    bool AudioManager::IsAudioLoaded(AudioId id) const {
        return m_loadedAudio.find(id) != m_loadedAudio.end();
    }

    TrackId AudioManager::CreateTrack() {
        if (!m_mixer) return INVALID_TRACK_ID;

        MIX_Track *track = MIX_CreateTrack(m_mixer);
        if (!track) {
            ENGINE_LOG_ERROR("Failed to create track! SDL_mixer Error: %s", SDL_GetError());
            return INVALID_TRACK_ID;
        }

        TrackId id = m_nextTrackId++;
        m_tracks[id] = track;
        return id;
    }

    void AudioManager::DestroyTrack(TrackId id) {
        auto it = m_tracks.find(id);
        if (it != m_tracks.end()) {
            MIX_DestroyTrack(it->second);
            m_tracks.erase(it);
            m_trackTags.erase(id);
            m_trackGains.erase(id);
        }
    }

    void AudioManager::SetTrackAudio(TrackId trackId, AudioId audioId) {
        auto trackIt = m_tracks.find(trackId);
        auto audioIt = m_loadedAudio.find(audioId);

        if (trackIt != m_tracks.end() && audioIt != m_loadedAudio.end()) {
            MIX_SetTrackAudio(trackIt->second, audioIt->second);
        } else {
            ENGINE_LOG_ERROR("SetTrackAudio: Invalid trackId (%u) or audioId (%u)", trackId, audioId);
        }
    }

    bool AudioManager::PlayTrack(TrackId trackId) {
        return PlayTrack(trackId, PlayOptions());
    }

    bool AudioManager::PlayTrack(TrackId trackId, const PlayOptions &options) {
        auto it = m_tracks.find(trackId);
        if (it == m_tracks.end()) return false;

        MIX_Track *track = it->second;

        if (options.tag != NO_TAG) {
            TagTrack(trackId, options.tag);
        }

        SetTrackGain(trackId, options.gain);

        SDL_PropertiesID props = SDL_CreateProperties();

        if (options.loop == LoopMode::Loop) {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        } else if (options.loop == LoopMode::Count) {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, options.loopCount);
        } else {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, 0);
        }

        if (options.fadeInMs > 0) {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, options.fadeInMs);
        }


        if (options.startFrame > 0) {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_START_FRAME_NUMBER, options.startFrame);
        }

        if (options.endFrame > 0) {
            SDL_SetNumberProperty(props, MIX_PROP_PLAY_MAX_FRAME_NUMBER, options.endFrame);
        }

        bool result = MIX_PlayTrack(track, props);
        SDL_DestroyProperties(props);

        return result;
    }

    void AudioManager::StopTrack(TrackId trackId) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_StopTrack(it->second, 0);
        }
    }

    void AudioManager::StopTrack(TrackId trackId, Uint32 fadeOutMs) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            Sint64 fadeOutFrames = MIX_TrackMSToFrames(it->second, fadeOutMs);
            MIX_StopTrack(it->second, fadeOutFrames >= 0 ? fadeOutFrames : 0);
        }
    }

    void AudioManager::PauseTrack(TrackId trackId) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_PauseTrack(it->second);
        }
    }

    void AudioManager::ResumeTrack(TrackId trackId) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_ResumeTrack(it->second);
        }
    }

    bool AudioManager::IsTrackPlaying(TrackId trackId) const {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            return MIX_TrackPlaying(it->second) && !MIX_TrackPaused(it->second);
        }
        return false;
    }

    bool AudioManager::IsTrackPaused(TrackId trackId) const {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            return MIX_TrackPaused(it->second);
        }
        return false;
    }

    void AudioManager::SetTrackGain(TrackId trackId, float gain) {
        m_trackGains[trackId] = gain;
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_SetTrackGain(it->second, gain);
        }
    }

    float AudioManager::GetTrackGain(TrackId trackId) const {
        auto it = m_trackGains.find(trackId);
        if (it != m_trackGains.end()) {
            return it->second;
        }
        return 1.0f;
    }

    void AudioManager::SetMasterGain(float gain) {
        m_masterGain = gain;
        if (m_mixer) {
            MIX_SetMixerGain(m_mixer, gain);
        }
    }

    float AudioManager::GetMasterGain() const {
        return m_masterGain;
    }

    void AudioManager::SetTrack3DPosition(TrackId trackId, const AudioPosition3D &position) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_Point3D p;
            p.x = position.x;
            p.y = position.y;
            p.z = position.z;
            MIX_SetTrack3DPosition(it->second, &p);
        }
    }

    void AudioManager::ClearTrack3DPosition(TrackId trackId) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_SetTrack3DPosition(it->second, nullptr);
        }
    }

    void AudioManager::SetTrackStereoPan(TrackId trackId, const StereoPan &pan) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_StereoGains g;
            g.left = pan.left;
            g.right = pan.right;
            MIX_SetTrackStereo(it->second, &g);
        }
    }

    void AudioManager::ClearTrackStereoPan(TrackId trackId) {
        auto it = m_tracks.find(trackId);
        if (it != m_tracks.end()) {
            MIX_SetTrackStereo(it->second, nullptr);
        }
    }

    void AudioManager::TagTrack(TrackId trackId, AudioTag tag) {
        if (tag == NO_TAG) {
            m_trackTags.erase(trackId);
        } else {
            m_trackTags[trackId] = tag;
        }
    }

    void AudioManager::StopByTag(AudioTag tag) {
        for (const auto &pair: m_trackTags) {
            if (pair.second == tag) {
                StopTrack(pair.first);
            }
        }
    }

    void AudioManager::StopByTag(AudioTag tag, Uint32 fadeOutMs) {
        for (const auto &pair: m_trackTags) {
            if (pair.second == tag) {
                StopTrack(pair.first, fadeOutMs);
            }
        }
    }

    void AudioManager::PauseByTag(AudioTag tag) {
        for (const auto &pair: m_trackTags) {
            if (pair.second == tag) {
                PauseTrack(pair.first);
            }
        }
    }

    void AudioManager::ResumeByTag(AudioTag tag) {
        for (const auto &pair: m_trackTags) {
            if (pair.second == tag) {
                ResumeTrack(pair.first);
            }
        }
    }

    void AudioManager::SetGainByTag(AudioTag tag, float gain) {
        for (const auto &pair: m_trackTags) {
            if (pair.second == tag) {
                SetTrackGain(pair.first, gain);
            }
        }
    }

    void AudioManager::PauseAll() {
        for (const auto &pair: m_tracks) {
            MIX_PauseTrack(pair.second);
        }
    }

    void AudioManager::ResumeAll() {
        for (const auto &pair: m_tracks) {
            MIX_ResumeTrack(pair.second);
        }
    }

    void AudioManager::StopAll() {
        for (const auto &pair: m_tracks) {
            MIX_StopTrack(pair.second, 0);
        }
    }
}
