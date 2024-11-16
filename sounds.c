#include "sounds.h"

MusicData menu_music = {
    .bpm=250,
    .loop=true,
    .length=32,
    .music_notes=(Beat[]) {
        //NOTE      OCT     LENGTH
        // Bar 1
        {NOTE_C,    4,      1},
        {NOTE_F,    4,      1},
        {NOTE_NONE, 0,      1},
        {NOTE_F,    4,      1},
        // Bar 2
        {NOTE_NONE, 0,      1},
        {NOTE_F,    4,      1},
        {NOTE_E,    4,      1},
        {NOTE_C,    4,      1},
        // Bar 3
        {NOTE_NONE, 0,      1},
        {NOTE_D,    4,      1},
        {NOTE_NONE, 0,      1},
        {NOTE_D,    4,      1},
        // Bar 4
        {NOTE_NONE, 0,      1},
        {NOTE_NONE, 0,      1},
        {NOTE_D,    4,      1},
        {NOTE_C,    4,      1},
        // Bar 5
        {NOTE_C,    4,      1},
        {NOTE_D,    4,      1},
        {NOTE_NONE, 0,      1},
        {NOTE_D,    4,      1},
        // Bar 6
        {NOTE_NONE, 0,      1},
        {NOTE_E,    4,      1},
        {NOTE_F,    4,      1},
        {NOTE_E,    4,      1},
        // Bar 7
        {NOTE_NONE, 0,      1},
        {NOTE_F,    4,      1},
        {NOTE_NONE, 0,      1},
        {NOTE_F,    4,      1},
        // Bar 8
        {NOTE_F,    4,      1},
        {NOTE_E,    4,      1},
        {NOTE_D,    4,      1},
        {NOTE_C,    4,      1},
    }
};