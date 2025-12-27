#pragma once

#include <notification/notification.h>

// Macro to scale the note length to a 16-bit integer (0-4 range)
#define SCALE_NOTE_LENGTH(note_length) \
        ((uint16_t)(((note_length) / 4.0f) * 65535.0f))

// Macro to encode note, octave, and note_length (scaled [0-4]) into a single 32-bit number
#define ENCODE_NOTE(note, octave, note_length) \
        (((uint32_t)(note) << 26) | ((uint32_t)(octave) << 16) | (uint32_t)(SCALE_NOTE_LENGTH(note_length)))

typedef enum {
    NOTE_C = 0,
    NOTE_CS = 1,
    NOTE_D = 2,
    NOTE_DS = 3,
    NOTE_E = 4,
    NOTE_F = 5,
    NOTE_FS = 6,
    NOTE_G = 7,
    NOTE_GS = 8,
    NOTE_A = 9,
    NOTE_AS = 10,
    NOTE_B = 11,

    NOTE_END = 12,
    NOTE_BUZZ = 13,
    NOTE_NONE = 14,
} Note;

typedef struct {
    Note note;
    uint8_t octave;
    float beat_length;
} Beat;


typedef struct {
    uint32_t *music_notes;  // list of the notes, always end with NOTE_END!!
    uint8_t bpm;        // beats per minute
    float separation;   // note separation, shortens the notes so they can have a small gap (1 = 1 whole beat)
    bool loop;          // automatically restart the music if it reaches the end
} MusicData;

void set_audio(MusicData *music);

void play_audio();

void update_audio();

void stop_audio();

void setup_audio(NotificationApp *notification_app);

void set_volume(float volume);