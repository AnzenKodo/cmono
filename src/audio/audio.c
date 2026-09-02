// #define MINIAUDIO_IMPLEMENTATION
// #include "./external/miniaudio.h"

// ak: Helpers
//=============================================================================

internal Audio_Handle audio_handle_zero(void)
{
    return StructZeroType(Audio_Handle);
}

internal Audio_Handle _audio_handle_from_sound(_Audio_Sound *sound)
{
    Audio_Handle handle = {(uintptr_t)sound};
    return handle;
}

internal _Audio_Sound *_audio_sound_from_handle(Audio_Handle handle)
{
    _Audio_Sound *sound = (_Audio_Sound *)handle.u64[0];
    return sound;
}

internal Audio_Handle _audio_handle_from_voice(_Audio_Voice *voice)
{
    Audio_Handle handle = {(uintptr_t)voice};
    return handle;
}

internal _Audio_Voice *_audio_voice_from_handle(Audio_Handle handle)
{
    _Audio_Voice *voice = (_Audio_Voice *)handle.u64[0];
    return voice;
}

internal bool audio_handle_is_valid(Audio_Handle handle)
{
    return _audio_sound_from_handle(handle) != 0;
}

internal void _audio_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
    uint32_t channels = _audio_state->decoder.channels;
    float *out_buf = (float *)output;
    mem_set(out_buf, 0, frame_count * channels * sizeof(float));
    
    ma_mutex_lock(&_audio_state->mutex);
    
    _Audio_Voice *voice = _audio_state->active_voices_first;
    while(voice != NULL)
    {
        _Audio_Voice *next_voice = voice->next;
        
        if(voice->state == _Audio_Voice_State_Playing && voice->decoder_valid)
        {
            #define MIX_BUFFER_SIZE 1024
            float mix_buf[MIX_BUFFER_SIZE * 8];
            
            uint32_t out_channels = channels;
            if (out_channels > 8) out_channels = 8;
            
            ma_uint32 frames_mixed = 0;
            while(frames_mixed < frame_count)
            {
                ma_uint32 chunk_frames = frame_count - frames_mixed;
                if(chunk_frames > MIX_BUFFER_SIZE)
                {
                    chunk_frames = MIX_BUFFER_SIZE;
                }
                
                ma_uint64 frames_read = 0;
                ma_result result = ma_decoder_read_pcm_frames(&voice->decoder, mix_buf, chunk_frames, &frames_read);
                
                float volume = voice->params.volume;
                float left_gain = volume;
                float right_gain = volume;
                if(out_channels == 2)
                {
                    float pan = voice->params.pan;
                    if(pan < 0.f)
                    {
                        right_gain *= (1.f + pan);
                    }
                    else if(pan > 0.f)
                    {
                        left_gain *= (1.f - pan);
                    }
                }
                
                for(ma_uint32 i = 0; i < frames_read; i++)
                {
                    for(uint32_t c = 0; c < out_channels; c++)
                    {
                        float sample = mix_buf[i * out_channels + c];
                        if(out_channels == 2)
                        {
                            sample *= (c == 0) ? left_gain : right_gain;
                        }
                        else
                        {
                            sample *= volume;
                        }
                        out_buf[(frames_mixed + i) * channels + c] += sample;
                    }
                }
                
                frames_mixed += (ma_uint32)frames_read;
                
                if(result != MA_SUCCESS || frames_read < chunk_frames)
                {
                    if(voice->params.flags & Audio_Play_Flag_Loop)
                    {
                        ma_decoder_seek_to_pcm_frame(&voice->decoder, 0);
                    }
                    else
                    {
                        voice->state = _Audio_Voice_State_Finished;
                        break;
                    }
                }
            }
        }
        
        if(voice->state == _Audio_Voice_State_Finished)
        {
            DLLRemove(_audio_state->active_voices_first, _audio_state->active_voices_last, voice);
            if(voice->decoder_valid)
            {
                ma_decoder_uninit(&voice->decoder);
                voice->decoder_valid = false;
            }
            voice->state = _Audio_Voice_State_Inactive;
            SLLStackPush(_audio_state->free_voice, voice);
        }
        
        voice = next_voice;
    }
    
    ma_mutex_unlock(&_audio_state->mutex);
    (void)input;
    (void)device;
}

// ak: Play Parameters
//=============================================================================

internal Audio_Play_Params audio_play_params_default(void)
{
    return (Audio_Play_Params) {
        .volume = 1.f,
        .pitch  = 1.f,
        .pan    = 0.f,
        .bus    = 0,
        .flags  = 0,
    };
}

// ak: Lifetime
//=============================================================================

internal bool audio_init(uint32_t sample_rate, uint32_t channel_count)
{
    bool result = false;
    
    // ak: initialize sate
    Arena *arena = arena_alloc();
    _audio_state = arena_push(arena, _Audio_State, 1);
    _audio_state->arena = arena;
    _audio_state->decoder.format = ma_format_f32;
    _audio_state->decoder.channels = channel_count;
    _audio_state->decoder.sample_rate = sample_rate;
    
    // ak: initialize device config
    ma_device_config config  = ma_device_config_init(ma_device_type_playback);
    // Set to ma_format_unknown to use the device's native format.
    config.playback.format   = _audio_state->decoder.format;
    // Set to 0 to use the device's native channel count.
    config.playback.channels = _audio_state->decoder.channels;
    // Set to 0 to use the device's native sample rate.
    config.sampleRate        = _audio_state->decoder.sample_rate;
    config.dataCallback      = _audio_callback;

    // ak: initialize device
    ma_result init_result = ma_device_init(NULL, &config, &_audio_state->device);
    if (init_result == MA_SUCCESS)
    {
        ma_result mutex_result = ma_mutex_init(&_audio_state->mutex);
        if (mutex_result == MA_SUCCESS)
        {
            ma_result start_result = ma_device_start(&_audio_state->device);
            if(start_result == MA_SUCCESS)
            {
                result = true;
            }
            else
            {
                ma_mutex_uninit(&_audio_state->mutex);
                ma_device_uninit(&_audio_state->device);
            }
        }
        else
        {
            ma_device_uninit(&_audio_state->device);
        }
    }
    
    return result;
}

internal void audio_cleanup(void)
{
    ma_device_uninit(&_audio_state->device);
    ma_mutex_uninit(&_audio_state->mutex);
    arena_free(_audio_state->arena);
}

// ak: Loading
//=============================================================================

internal Audio_Handle audio_load_from_memory(void *data, size_t size, Audio_Load_Flags flags)
{
    if (data == NULL || size == 0)
    {
        return audio_handle_zero();
    }
    
    // ak: allocate sound record
    _Audio_Sound *sound = _audio_state->free_sound;
    if (sound)
    {
        SLLStackPop(_audio_state->free_sound);
    }
    else
    {
        sound = arena_push(_audio_state->arena, _Audio_Sound, 1);
    }
    
    // ak: fill sound data
    sound->in_use = true;
    sound->data   = data;
    sound->size   = size;
    sound->flags  = flags;
    
    // ak: bundle & return
    Audio_Handle result = _audio_handle_from_sound(sound);
    return result;
}

internal Audio_Handle audio_load_from_path(Str8 path, Audio_Load_Flags flags)
{
    void *data = NULL;
    size_t size = os_path_read_full(path, &data, _audio_state->arena);
    Audio_Handle result = audio_load_from_memory(data, size, flags);
    return result;
}

internal void audio_unload(Audio_Handle handle)
{
    _Audio_Sound *sound = _audio_sound_from_handle(handle);
    if(sound != 0)
    {
        ma_mutex_lock(&_audio_state->mutex);
        _Audio_Voice *voice = _audio_state->active_voices_first;
        while(voice != 0)
        {
            _Audio_Voice *next_voice = voice->next;
            if(voice->audio.u64[0] == handle.u64[0])
            {
                DLLRemove(_audio_state->active_voices_first, _audio_state->active_voices_last, voice);
                if(voice->decoder_valid)
                {
                    ma_decoder_uninit(&voice->decoder);
                    voice->decoder_valid = false;
                }
                voice->state = _Audio_Voice_State_Inactive;
                SLLStackPush(_audio_state->free_voice, voice);
            }
            voice = next_voice;
        }
        
        sound->in_use = false;
        sound->data = NULL;
        sound->size = 0;
        SLLStackPush(_audio_state->free_sound, sound);
        ma_mutex_unlock(&_audio_state->mutex);
    }
}

// ak: Playback
//=============================================================================

internal Audio_Handle audio_play(Audio_Handle audio, Audio_Play_Params params)
{
    _Audio_Sound *sound = _audio_sound_from_handle(audio);
    if(sound == 0 || !sound->in_use) { return audio_handle_zero(); }
    
    ma_mutex_lock(&_audio_state->mutex);
    
    _Audio_Voice *voice = _audio_state->free_voice;
    if (voice)
    {
        SLLStackPop(_audio_state->free_voice);
    }
    else
    {
        voice = arena_push(_audio_state->arena, _Audio_Voice, 1);
    }
    
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, _audio_state->decoder.channels, _audio_state->decoder.sample_rate);
    ma_result decode_result = ma_decoder_init_memory(sound->data, sound->size, &config, &voice->decoder);
    if(decode_result == MA_SUCCESS)
    {
        voice->decoder_valid = true;
        voice->audio        = audio;
        voice->params       = params;
        voice->state        = (params.flags & Audio_Play_Flag_StartPaused)
            ? _Audio_Voice_State_Paused : _Audio_Voice_State_Playing;
            
        DLLPushBack(_audio_state->active_voices_first, _audio_state->active_voices_last, voice);
    }
    else
    {
        voice->decoder_valid = false;
        SLLStackPush(_audio_state->free_voice, voice);
        voice = NULL;
    }
    
    ma_mutex_unlock(&_audio_state->mutex);
    
    return voice ? _audio_handle_from_voice(voice) : audio_handle_zero();
}
