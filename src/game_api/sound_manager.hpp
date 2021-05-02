#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "fmod.hpp"
#include "audio_buffer.hpp"

namespace sol {
    template<bool b>
    class basic_reference;
    using reference = basic_reference<false>;
    template<typename T, bool, typename H>
    class basic_protected_function;
    using safe_function = basic_protected_function<reference, false, reference>;
    using protected_function = safe_function;
    using function = protected_function;
}


class SoundManager;
class PlayingSound;

using SoundCallbackFunction = sol::function;

enum class LoopMode {
    Off,
    Loop,
    Bidirectional
};

enum class SoundType {
    Sfx,
    Music
};

class CustomSound
{
    friend class SoundManager;

public:
    CustomSound(const CustomSound& rhs);
    CustomSound(CustomSound&& rhs) noexcept;
    CustomSound& operator=(const CustomSound& rhs) = delete;
    CustomSound& operator=(CustomSound&& rhs) = delete;
    ~CustomSound();

    operator bool() { return m_SoundManager != nullptr; }

    PlayingSound play();
    PlayingSound play(bool paused);
    PlayingSound play(bool paused, SoundType sound_type);

private:
    CustomSound(std::nullptr_t, std::nullptr_t) {}
    CustomSound(FMOD::Sound* fmod_sound, SoundManager* sound_manager);
    CustomSound(FMODStudio::EventDescription* fmod_event, SoundManager* sound_manager);

    std::variant<FMOD::Sound*, FMODStudio::EventDescription*, std::monostate> m_FmodHandle{};
    SoundManager* m_SoundManager{ nullptr };
};

using PlayingSoundHandle = std::variant<FMOD::Channel*, FMODStudio::EventInstance*, std::monostate>;
class PlayingSound {
    friend class SoundManager;
    friend class CustomSound;

public:
    PlayingSound(const PlayingSound& rhs) = default;
    PlayingSound(PlayingSound&& rhs) noexcept = default;
    PlayingSound& operator=(const PlayingSound& rhs) = default;
    PlayingSound& operator=(PlayingSound&& rhs) noexcept = default;
    ~PlayingSound() = default;

    bool is_playing();
    bool stop();
    bool set_pause(bool pause);
    bool set_mute(bool mute);
    bool set_pitch(float pitch);
    bool set_pan(float pan);
    bool set_volume(float volume);
    bool set_looping(LoopMode loop_mode);
    bool set_callback(SoundCallbackFunction&& callback);

private:
    PlayingSound(std::nullptr_t, std::nullptr_t) {}
    PlayingSound(FMOD::Channel* fmod_channel, SoundManager* sound_manager);
    PlayingSound(FMODStudio::EventInstance* fmod_event, SoundManager* sound_manager);

    PlayingSoundHandle m_FmodHandle{};
    SoundManager* m_SoundManager{ nullptr };
};

class SoundManager
{
public:
    SoundManager(DecodeAudioFile* decode_function);
    ~SoundManager();

    SoundManager(const SoundManager&) = delete;
    SoundManager(SoundManager&&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    SoundManager& operator=(SoundManager&&) = delete;

    CustomSound get_sound(std::string path);
    CustomSound get_sound(const char* path);
    CustomSound get_existing_sound(std::string_view path);
    void acquire_sound(FMOD::Sound* fmod_sound);
    void release_sound(FMOD::Sound* fmod_sound);
    PlayingSound play_sound(FMOD::Sound* fmod_sound, bool paused, bool as_music);

    CustomSound get_event(std::string_view event_name);
    PlayingSound play_event(FMODStudio::EventDescription* fmod_event, bool paused, bool as_music);

    bool is_playing(PlayingSound playing_sound);
    bool stop(PlayingSound playing_sound);
    bool set_pause(PlayingSound playing_sound, bool pause);
    bool set_mute(PlayingSound playing_sound, bool mute);
    bool set_pitch(PlayingSound playing_sound, float pitch);
    bool set_pan(PlayingSound playing_sound, float pan);
    bool set_volume(PlayingSound playing_sound, float volume);
    bool set_looping(PlayingSound playing_sound, LoopMode loop_mode);
    bool set_callback(PlayingSound playing_sound, SoundCallbackFunction&& callback);

private:
    DecodeAudioFile* m_DecodeFunction{ nullptr };

    FMOD::System* m_FmodSystem{ nullptr };

    FMOD::CreateSound* m_CreateSound{ nullptr };
    FMOD::ReleaseSound* m_ReleaseSound{ nullptr };
    FMOD::PlaySound* m_PlaySound{ nullptr };

    FMOD::ChannelIsPlaying* m_ChannelIsPlaying{ nullptr };
    FMOD::ChannelStop* m_ChannelStop{ nullptr };
    FMOD::ChannelSetPaused* m_ChannelSetPaused{ nullptr };
    FMOD::ChannelSetMute* m_ChannelSetMute{ nullptr };
    FMOD::ChannelSetPitch* m_ChannelSetPitch{ nullptr };
    FMOD::ChannelSetPan* m_ChannelSetPan{ nullptr };
    FMOD::ChannelSetVolume* m_ChannelSetVolume{ nullptr };
    FMOD::ChannelSetMode* m_ChannelSetMode{ nullptr };
    FMOD::ChannelSetCallback* m_ChannelSetCallback{ nullptr };
    FMOD::ChannelSetUserData* m_ChannelSetUserData{ nullptr };
    FMOD::ChannelGetUserData* m_ChannelGetUserData{ nullptr };

    FMODStudio::EventDescriptionCreateInstance* m_EventCreateInstance{ nullptr };
    FMODStudio::EventDescriptionGetParameterDescriptionByID* m_EventDescriptionGetParameterDescriptionByID{ nullptr };
    FMODStudio::EventDescriptionGetParameterDescriptionByName* m_EventDescriptionGetParameterDescriptionByName{ nullptr };

    FMODStudio::EventInstanceStart* m_EventInstanceStart{ nullptr };
    FMODStudio::EventInstanceStop* m_EventInstanceStop{ nullptr };
    FMODStudio::EventInstanceGetPlaybackState* m_EventInstanceGetPlaybackState{ nullptr };
    FMODStudio::EventInstanceSetPaused* m_EventInstanceSetPaused{ nullptr };
    FMODStudio::EventInstanceGetPaused* m_EventInstanceGetPaused{ nullptr };
    FMODStudio::EventInstanceSetPitch* m_EventInstanceSetPitch{ nullptr };
    FMODStudio::EventInstanceSetVolume* m_EventInstanceSetVolume{ nullptr };
    FMODStudio::EventInstanceSetCallback* m_EventInstanceSetCallback{ nullptr };
    FMODStudio::EventInstanceSetUserData* m_EventInstanceSetUserData{ nullptr };
    FMODStudio::EventInstanceGetUserData* m_EventInstanceGetUserData{ nullptr };

    FMOD::ChannelGroup* m_SfxChannelGroup{ nullptr };
    FMOD::ChannelGroup* m_MusicChannelGroup{ nullptr };

    using EventId = std::uint32_t;
    struct EventParameters {
        std::string ParameterNames[38];
    };
    struct EventDescription {
        FMODStudio::EventDescription* Event;
        EventId Id;
        std::string Name;
        FMODStudio::ParameterId Parameters[38];
        bool HasParameter[38];
        std::uint64_t _ull0;
        std::uint32_t _u0;
        std::uint32_t _u1;
        std::uint32_t _u2;
    };
    using EventMap = std::unordered_map<EventId, EventDescription>;
    struct SoundData {
        const EventParameters* Parameters;
        const EventMap* Events;
        std::unordered_map<std::string_view, const EventDescription*> NameToEvent;
    };
    static_assert(sizeof(EventDescription) == 0x1a0);
    SoundData m_SoundData;

    struct Sound;
    std::vector<Sound> m_SoundStorage;
};
