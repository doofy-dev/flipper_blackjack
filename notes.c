#include <notification/notification_messages.h>
#include "notes.h"


const NotificationMessage note_c4 = {
    .data.sound.volume = VOLUME,
    .data.sound.frequency = 261.63f,
    .type = NotificationMessageTypeSoundOn,
};
const NotificationMessage note_d4 = {
    .data.sound.volume = VOLUME,
    .data.sound.frequency = 293.66f,
    .type = NotificationMessageTypeSoundOn,
};
const NotificationMessage note_e4 = {
    .data.sound.volume = VOLUME,
    .data.sound.frequency = 329.63f,
    .type = NotificationMessageTypeSoundOn,
};
const NotificationMessage note_f4 = {
    .data.sound.volume = VOLUME,
    .data.sound.frequency = 349.23f,
    .type = NotificationMessageTypeSoundOn,
};

const NotificationMessage *music_notes[] = {
//-------- Bar 1 --------
    &note_c4,
    &note_f4,
    &message_sound_off,
    &note_f4,
//-------- Bar 2 --------
    &message_sound_off,
    &note_f4,
    &note_e4,
    &note_c4,
//-------- Bar 3 --------
    &message_sound_off,
    &note_d4,
    &message_sound_off,
    &note_d4,
//-------- Bar 4 --------
    &message_sound_off,
    &message_sound_off,
    &note_d4,
    &note_c4,
//-------- Bar 5 --------
    &note_c4,
    &note_d4,
    &message_sound_off,
    &note_d4,
//-------- Bar 6 --------
    &message_sound_off,
    &note_e4,
    &note_f4,
    &note_e4,
//-------- Bar 7 --------
    &message_sound_off,
    &note_f4,
    &message_sound_off,
    &note_f4,
//-------- Bar 8 --------
    &note_f4,
    &note_e4,
    &note_d4,
    &note_c4,
};

NotificationSequence music = {
    NULL,
    &message_delay_250,
    &message_sound_off,
    NULL
};