#pragma once

#include <furi.h>
#include <gui/canvas.h>
#include "../math/vector.h"

typedef struct Buffer Buffer;

struct Buffer {
    uint8_t *data;
    uint8_t *back_buffer;
    uint16_t width;
    uint16_t real_width;
    uint16_t height;
    bool double_buffered;
    bool clear;
};

typedef enum  {
    COLOR_BLACK, //or
    COLOR_WHITE, //
    COLOR_FLIP, //not
    COLOR_SET,
    COLOR_TRANSPARENT
} PixelColor;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Buffer *buffer_create(uint8_t width, uint8_t height, bool double_buffered);

void buffer_release(Buffer *buffer);

bool buffer_test_coordinate(Buffer *const buffer, int x, int y);

bool buffer_read_pixel(Buffer *const buffer, int x, int y);

void buffer_set_pixel(Buffer *buffer, int16_t x, int16_t y, PixelColor color);


bool buffer_get_relative_pixel(Buffer *const buffer, int x, int y);

void buffer_set_relative_pixel(Buffer *buffer, int16_t x, int16_t y, PixelColor color);

Buffer *buffer_copy(Buffer *buffer);

void buffer_swap_back(Buffer *buffer);

void buffer_swap_with(Buffer *buffer_a, Buffer *buffer_b);

void buffer_render(Buffer *buffer, Canvas *const canvas);

void buffer_clear(Buffer *buffer);

Buffer *buffer_decompress_icon(const Icon *icon);
bool buffer_sample(Buffer *buffer, Vector *uv);

//if needed, rounded rectangle can be achieved by creating a new sampling for render_filled function and base it on UV
