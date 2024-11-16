#pragma once


#include <notification/notification.h>



typedef enum {
    NOTE_BUZZ = -2,
    NOTE_NONE = -1,
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
} Note;

typedef struct {
    Note note;
    uint8_t octave;
    float beat_length;
} Beat;


typedef struct {
    Beat *music_notes;
    uint32_t length;
    uint8_t bpm;
    bool loop;
} MusicData;

void set_audio(MusicData *music);

void play_audio();

void update_audio(NotificationApp *notification_app);

void stop_audio();

void set_volume(float volume);