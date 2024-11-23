#include "sounds.h"

// Macro to scale the note length to a 16-bit integer (0-4 range)
#define SCALE_NOTE_LENGTH(note_length) \
        ((uint16_t)(((note_length) / 4.0f) * 65535.0f))

// Macro to encode note, octave, and note_length (scaled [0-4]) into a single 32-bit number
#define ENCODE_NOTE(note, octave, note_length) \
        (((uint32_t)(note) << 26) | ((uint32_t)(octave) << 16) | (uint32_t)(SCALE_NOTE_LENGTH(note_length)))

MusicData bg_music = {
    .bpm=100,
    .loop=true,
    .separation=0.05f,
    .music_notes=(uint32_t []) {
                    //NOTE      OCT     LENGTH
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_D, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_A, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_C, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.5f),

        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_G, 3, 0.25f),
        ENCODE_NOTE(NOTE_NONE, 0, 0.25f),

        ENCODE_NOTE(NOTE_END, 0, 0),
    }
};

