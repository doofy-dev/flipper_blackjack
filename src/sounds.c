#include "sounds.h"

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

