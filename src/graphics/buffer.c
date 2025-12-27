#include "buffer.h"
#include "../util/helpers.h"
#include "../math/matrix.h"
#include "../math/equation.h"
#include <furi.h>
#include <toolbox/compress.h>

uint16_t pixel(uint8_t x, uint8_t y, uint8_t w) {
    return (y * w + x) / 8;
}

unsigned long buffer_size(uint8_t width, uint8_t height) {
    return sizeof(uint8_t) * (int) ceil(width / 8.0) * ceil(height);
}

uint8_t *malloc_buffer(uint8_t width, uint8_t height) {
    return (uint8_t *) allocate(buffer_size(width, height));
}

Buffer *buffer_create(uint8_t width, uint8_t height, bool double_buffered) {
    Buffer *b = (Buffer *) allocate(sizeof(Buffer));
    check_pointer(b);
    b->double_buffered = double_buffered;
    b->width = width;
    b->real_width = ceil(width / 8.0);
    b->height = height;
    b->data = malloc_buffer(width, height);
    check_pointer(b->data);
    if (double_buffered) {
        b->back_buffer = malloc_buffer(width, height);
        check_pointer(b->back_buffer);
    } else
        b->back_buffer = NULL;
    return b;
}

void buffer_release(Buffer *buffer) {
    release(buffer->data);
    if (buffer->double_buffered)
        release(buffer->back_buffer);
    release(buffer);
}

bool buffer_test_coordinate(Buffer *const buffer, int x, int y) {
    return (x >= 0 && x < buffer->width && y >= 0 && y < buffer->height);
}

bool buffer_read_pixel(Buffer *const buffer, int x, int y) {
    return buffer->data[pixel(x, y, buffer->width)] & (1 << (x & 7));
}

void buffer_set_pixel_with_check(Buffer *buffer, int16_t x, int16_t y, PixelColor color) {
    if (buffer_test_coordinate(buffer, x, y))
        buffer_set_pixel(buffer, x, y, color);
}

void buffer_set_pixel(Buffer *buffer, int16_t x, int16_t y, PixelColor color) {
    uint8_t bit = 1 << (x & 7);
    uint8_t *p = &(buffer->data[pixel(x, y, buffer->width)]);

    switch (color) {
        case COLOR_BLACK:
            *p |= bit;
            break;
        case COLOR_WHITE:
            *p &= ~bit;
            break;
        case COLOR_FLIP:
            *p ^= bit;
            break;
        default:
            //Set is not handled here
            break;
    }
}

Buffer *buffer_copy(Buffer *buffer) {
    Buffer *new_buffer = (Buffer *) allocate(sizeof(Buffer));
    new_buffer->double_buffered = buffer->double_buffered;
    new_buffer->width = buffer->width;
    new_buffer->height = buffer->height;
    new_buffer->data = malloc_buffer(buffer->width, buffer->height);
    memcpy(new_buffer->data, buffer->data, buffer_size(buffer->width, buffer->height));
    if (buffer->double_buffered) {
        new_buffer->back_buffer = malloc_buffer(buffer->width, buffer->height);
        memcpy(new_buffer->back_buffer, buffer->back_buffer, buffer_size(buffer->width, buffer->height));
    } else {
        new_buffer->back_buffer = NULL;
    }
    return new_buffer;
}

void buffer_swap_back(Buffer *buffer) {
    check_pointer(buffer);
    check_pointer(buffer->data);
    if (buffer->double_buffered) {
        check_pointer(buffer->back_buffer);
        uint8_t *temp = buffer->data;
        buffer->data = buffer->back_buffer;
        buffer->back_buffer = temp;
    }
}

void buffer_clear(Buffer *buffer) {
    check_pointer(buffer);
    check_pointer(buffer->data);
    buffer->data = (uint8_t *) memset(buffer->data, 0, buffer_size(buffer->width, buffer->height));
}

void buffer_swap_with(Buffer *buffer_a, Buffer *buffer_b) {
    check_pointer(buffer_a);
    check_pointer(buffer_b);
    uint8_t *temp = buffer_a->data;
    buffer_a->data = buffer_b->data;
    buffer_b->data = temp;
}

void buffer_render(Buffer *buffer, Canvas *const canvas) {
    check_pointer(buffer);
    canvas_draw_xbm(canvas, 0, 0, buffer->width, buffer->height, buffer->data);
}

Buffer *buffer_decompress_icon(const Icon *icon) {
    uint8_t *p_icon_data;
    Buffer *b = malloc(sizeof(Buffer));

    b->real_width = icon_get_width(icon);
    b->width = (int) (ceil(b->real_width / 8.0) * 8);
    b->height = icon_get_height(icon);

    uint16_t size = buffer_size(b->width, b->height);
    b->data = malloc_buffer(b->width, b->height);

    CompressIcon *compress_icon = compress_icon_alloc(size);
    compress_icon_decode(compress_icon, icon_get_frame_data(icon, 0), &p_icon_data);

    memcpy(b->data, p_icon_data, size);
    compress_icon_free(compress_icon);

    return b;
}

bool buffer_sample(Buffer *buffer, Vector *uv) {
    int U = FLOOR(uv->x * buffer->real_width);
    int V = FLOOR(uv->y * buffer->height);

    if (U < 0 || U >= buffer->real_width || V < 0 || V >= buffer->height) return false;

    return buffer_read_pixel(buffer, U, V);
}
