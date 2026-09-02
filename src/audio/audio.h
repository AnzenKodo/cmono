#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_ALSA
// #define MA_ENABLE_PULSEAUDIO
// #define MA_ENABLE_JACK
#define MA_NO_ENGINE
#include "./external/miniaudio.h"

// ak: Types
//=============================================================================

// ak: Handles ================================================================

typedef struct Audio_Handle Audio_Handle;
struct Audio_Handle
{
    uint64_t u64[1];
};

// ak: Enums ==================================================================

typedef uint32_t Audio_Load_Flags;
enum
{
    Audio_Load_Flag_Stream    = (1<<0), // decode on the fly instead of loading fully into memory
    Audio_Load_Flag_Loop      = (1<<1), // default the audio to looping playback
};

typedef struct _Audio_Sound _Audio_Sound;
struct _Audio_Sound
{
    _Audio_Sound     *next;
    void             *data;
    size_t           size;
    Audio_Load_Flags flags;
    bool             in_use;
};

// ak: Play Parameters ========================================================

typedef uint32_t Audio_Play_Flags;
enum
{
    Audio_Play_Flag_Loop        = (1<<0),
    Audio_Play_Flag_StartPaused = (1<<1),
};

typedef struct Audio_Play_Params Audio_Play_Params;
struct Audio_Play_Params
{
    float            volume;      // 0 = silent, 1 = unity gain
    float            pitch;       // 1 = unmodified
    float            pan;         // -1 = full left, 0 = center, 1 = full right
    uint32_t         bus;         // index in [0, SND_MAX_BUSES)
    Audio_Play_Flags flags;
};

// ak: Voice ==================================================================

typedef uint32_t _Audio_Voice_State;
enum
{
    _Audio_Voice_State_Inactive,
    _Audio_Voice_State_Playing,
    _Audio_Voice_State_Paused,
    _Audio_Voice_State_Finished,
};

typedef struct _Audio_Voice _Audio_Voice;
struct _Audio_Voice
{
    _Audio_Voice       *next;
    _Audio_Voice       *prev;
    Audio_Handle       audio;
    _Audio_Voice_State state;
    Audio_Play_Params  params;
    ma_decoder         decoder;
    bool               decoder_valid;
};

// ak: State
//=============================================================================

typedef struct _Audio_State _Audio_State;
struct _Audio_State
{
    Arena *arena;
    ma_device device;
    ma_mutex mutex;
    struct {
        ma_format format;
        ma_uint32 channels;
        ma_uint32 sample_rate;
    } decoder;
    _Audio_Sound *free_sound;
    _Audio_Voice *free_voice;
    _Audio_Voice *active_voices_first;
    _Audio_Voice *active_voices_last;
};

// ak: Global
//=============================================================================

global _Audio_State *_audio_state = 0;
