#include "buffer.h"
#include "helpers.h"
#include "matrix.h"
#include <furi.h>
#include <toolbox/compress.h>

static enum DrawMode drawMode = BlackOnly;
static Matrix *current_transform;
static float current_sprite_rotation = 0;

uint16_t pixel(uint8_t x, uint8_t y, uint8_t w) {
    return (y * w + x) / 8;
}

void buffer_set_sprite_rotation(float rotation) {
    current_sprite_rotation = rotation;
}

void buffer_set_transform(Matrix *transform) {
    current_transform = transform;
}

void buffer_set_draw_mode(enum DrawMode mode) {
    drawMode = mode;
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

bool buffer_get_absolute_pixel(Buffer *const buffer, int x, int y) {
    return buffer->data[pixel(x, y, buffer->width)] & (1 << (x & 7));
}

bool buffer_get_relative_pixel(Buffer *const buffer, int x, int y) {
    Vector curr = {x, y};
    Vector result;

    if (current_transform)
        matrix_mul_vector(current_transform, &curr, &result);
    else
        result = curr;

    int _x = (int) result.x;
    int _y = (int) result.y;

    if (buffer_test_coordinate(buffer, _x, _y)) {
        return buffer->data[pixel(_x, _y, buffer->width)] & (1 << (x & 7));
    }

    return false;
}

void buffer_set_pixel_with_check(Buffer *buffer, int16_t x, int16_t y, enum PixelColor color) {
    if (buffer_test_coordinate(buffer, x, y))
        buffer_set_absolute_pixel(buffer, x, y, color);
}


void buffer_set_relative_pixel(Buffer *buffer, int16_t x, int16_t y, enum PixelColor color) {
    Vector curr = {x, y};
    Vector result;

    if (current_transform)
        matrix_mul_vector(current_transform, &curr, &result);
    else
        result = curr;

    int _x = (int) result.x;
    int _y = (int) result.y;

    buffer_set_pixel_with_check(buffer, _x, _y, color);
}

void buffer_set_absolute_pixel(Buffer *buffer, int16_t x, int16_t y, enum PixelColor color) {
    uint8_t bit = 1 << (x & 7);
    uint8_t *p = &(buffer->data[pixel(x, y, buffer->width)]);

    switch (color) {
        case Black:
            *p |= bit;
            break;
        case White:
            *p &= ~bit;
            break;
        case Flip:
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

///*
// TODO: decide if this is better than scaling the sampling
bool average_pixel(Buffer *const sprite, int x, int y, bool rotated) {
    if (!rotated) {
        return buffer_get_absolute_pixel(sprite, x, y);
    }
    float val = 0;
    int sample_rate = 1;

    for (int sample_x = x - sample_rate; sample_x < x + sample_rate; sample_x++) {
        for (int sample_y = y - sample_rate; sample_y < y + sample_rate; sample_y++) {
            if (buffer_test_coordinate(sprite, sample_x, sample_y)) {
                bool px = buffer_get_absolute_pixel(sprite, x, y);
                val += px ? 1 : 0;
            }
        }
    }

    return (val / 9.f) > 0.15f;
}//*/



void
buffer_draw_internal(Buffer *target, Buffer *const sprite, bool is_black, enum PixelColor color, Vector *const position,
                     uint8_t x_limit, uint8_t y_limit) {
    Vector transform, curr;

    bool isOn;
    int finalX, finalY;
    uint8_t x_cap = MIN(sprite->width, x_limit);
    uint8_t y_cap = MIN(sprite->height, y_limit);
    Vector scaling = {1, 1};
    float rotation = 0;


    if (current_transform) {
        matrix_get_scaling(current_transform, &scaling);
        rotation = matrix_get_rotation(current_transform);
    }
    rotation *= RAD_2_DEG;

    float cosrad = cosf(current_sprite_rotation*DEG_2_RAD);
    float sinrad = sinf(current_sprite_rotation*DEG_2_RAD);

    float x_inc = MAX(1 / scaling.x, 0.05f);
    float y_inc = MAX(1 / scaling.y, 0.05f);
    if ((int) rotation % 45 != 0 || ((int) (current_sprite_rotation)) % 45 != 0) {
        x_inc /= 1.5f;
        y_inc /= 1.5f;
    }

    float w2 = sprite->width / 2;
    float h2 = sprite->height / 2;
    float sw2 = scaling.x / w2;
    float sh2 = scaling.y / h2;
    int lastX = -1, lastY = -1;

    for (float y = 0; y < y_cap; y += y_inc) {
        for (float x = 0; x < x_cap; x += x_inc) {
            isOn = (lastX == (int) x && lastY == (int) y) ||
                   //                average_pixel(sprite, (int) x, (int) y, rotated) == is_black;
                   buffer_get_absolute_pixel(sprite, (int) (x), (int) (y)) == is_black;
            if (isOn || color == Set) {
                lastX = (int) x;
                lastY = (int) y;

                curr.x = (float) x - w2 + (1 - (sprite->width % 2));
                curr.y = (float) y - h2;
                //rotate sprite first to not need matrix swap frequently
                if (current_sprite_rotation != 0) {
                    float x,y;
                    x=cosrad * curr.x - sinrad*curr.y;
                    y=sinrad * curr.x + cosrad*curr.y;
                    curr.x=x;
                    curr.y=y;
//                    vector_rotate(&curr, current_sprite_rotation, &curr);
                }
                curr.x += position->x;
                curr.y += position->y;

                if (current_transform) {
                    matrix_mul_vector(current_transform, &curr, &transform);
                } else
                    transform = curr;

                transform.x += sw2;
                transform.y += sh2;

                finalX = (int) roundf(transform.x-0.11f);
                finalY = (int) roundf(transform.y-0.11f);
                if (buffer_test_coordinate(target, finalX, finalY)) {
                    if (isOn) {
                        buffer_set_absolute_pixel(target, finalX, finalY, color == Set ? Black : color);
                    } else {
                        buffer_set_absolute_pixel(target, finalX, finalY, White);
                    }
                }
            }
        }
    }

}

void buffer_draw_all(Buffer *target, Buffer *const sprite, Vector *position) {
    check_pointer(target);
    check_pointer(sprite);
    check_pointer(position);
    buffer_draw(target, sprite, position, sprite->width, sprite->height);
}

void
buffer_draw(Buffer *target, Buffer *const sprite, Vector *position, uint8_t x_cap, uint8_t y_cap) {
    check_pointer(target);
    check_pointer(sprite);
    check_pointer(position);
    switch (drawMode) {
        default:
        case All:
            buffer_draw_internal(target, sprite, true, Set, position, x_cap, y_cap);
            break;
        case BlackOnly:
            buffer_draw_internal(target, sprite, true, Black, position, x_cap, y_cap);
            break;
        case WhiteOnly:
            buffer_draw_internal(target, sprite, false, White, position, x_cap, y_cap);
            break;
        case WhiteAsBlack:
            buffer_draw_internal(target, sprite, false, Black, position, x_cap, y_cap);
            break;
        case BlackAsWhite:
            buffer_draw_internal(target, sprite, true, White, position, x_cap, y_cap);
            break;
        case WhiteAsInverted:
            buffer_draw_internal(target, sprite, false, Flip, position, x_cap, y_cap);
            break;
        case BlackAsInverted:
            buffer_draw_internal(target, sprite, true, Flip, position, x_cap, y_cap);
            break;
    }
}

void buffer_render(Buffer *buffer, Canvas *const canvas) {
    check_pointer(buffer);
    canvas_draw_xbm(canvas, 0, 0, buffer->width, buffer->height, buffer->data);
}

void buffer_draw_line(Buffer *buffer, int x0, int y0, int x1, int y1, enum PixelColor draw_mode) {
    check_pointer(buffer);
    Vector a, b, tmp;
    a = (Vector) {x0, y0};

    if (current_transform) {
        matrix_mul_vector(current_transform, &a, &tmp);
        a = tmp;
    }

    b = (Vector) {x1, y1};
    if (current_transform) {
        matrix_mul_vector(current_transform, &b, &tmp);
        b = tmp;
    }

    int16_t _x0 = (int16_t) a.x;
    int16_t _x1 = (int16_t) b.x;
    int16_t _y0 = (int16_t) a.y;
    int16_t _y1 = (int16_t) b.y;

    int dx = abs(_x1 - _x0);
    int16_t sx = _x0 < _x1 ? 1 : -1;

    int dy = abs(_y1 - _y0);
    int16_t sy = _y0 < _y1 ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2;

    while (true) {
        if (buffer_test_coordinate(buffer, _x0, _y0)) {
            buffer_set_absolute_pixel(buffer, _x0, _y0, draw_mode);
        }
        if (_x0 == _x1 && _y0 == _y1) break;
        int e2 = err;
        if (e2 > -dx) {
            err -= dy;
            _x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            _y0 += sy;
        }
    }
}

void buffer_draw_rbox(Buffer *buffer, int16_t x0, int16_t y0, int16_t x1, int16_t y1, enum PixelColor draw_mode) {

    Vector p, tmp;
    Vector scaling = {1, 1};
    float rotation = 0;

    if (current_transform) {
        matrix_get_scaling(current_transform, &scaling);
        rotation = matrix_get_rotation(current_transform);
    }

    float x_inc = MAX(1 / scaling.x, 0.1);
    float y_inc = MAX(1 / scaling.y, 0.1);

    if ((int) (rotation * RAD_2_DEG) % 30 != 0) {
        x_inc /= 2.0f;
        y_inc /= 2.0f;
    }

    for (float x = x0; x < x1; x += x_inc) {
        for (float y = y0; y < y1; y += y_inc) {
            tmp = (Vector) {x, y};
            p = (Vector) {x, y};

            if (current_transform) {
                matrix_mul_vector(current_transform, &tmp, &p);
            }

            if (((x == x0 || x == x1 - 1) && (y == y0 || y == y1 - 1)) ||
                !buffer_test_coordinate(buffer, p.x, p.y))
                continue;
            buffer_set_absolute_pixel(buffer, p.x, p.y, draw_mode);
        }
    }
}

void buffer_draw_rbox_frame(Buffer *buffer, int16_t x0, int16_t y0, int16_t x1, int16_t y1, enum PixelColor draw_mode) {
    buffer_draw_line(buffer, x0 + 1, y0, x1 - 1, y0, draw_mode);
    buffer_draw_line(buffer, x0 + 1, y1, x1 - 1, y1, draw_mode);

    buffer_draw_line(buffer, x0, y0 + 1, x0, y1 - 1, draw_mode);
    buffer_draw_line(buffer, x1, y0 + 1, x1, y1 - 1, draw_mode);
}

void buffer_draw_box(Buffer *buffer, int16_t x0, int16_t y0, int16_t x1, int16_t y1, enum PixelColor draw_mode) {
    Vector tmp = (Vector) {x0, y0};
    Vector s, t;
    if (current_transform) {
        matrix_mul_vector(current_transform, &tmp, &s);
        tmp = (Vector) {x1, y1};
        matrix_mul_vector(current_transform, &tmp, &t);
    } else {
        s = (Vector) {x0, y0};
        t = (Vector) {x1, y1};
    }

    if (s.x < t.x) {
        float a = s.x;
        s.x = t.x;
        t.x = a;
    }
    if (s.y < t.y) {
        float a = s.y;
        s.y = t.y;
        t.y = a;
    }

    for (int16_t x = s.x + 1; x < t.x - 1; x++) {
        for (int16_t y = s.y + 1; y < t.y - 1; y++) {
            if (!buffer_test_coordinate(buffer, x, y))
                continue;
            buffer_set_absolute_pixel(buffer, x, y, draw_mode);
        }
    }
}

Buffer* buffer_decompress_icon(const Icon* icon){
    uint8_t* p_icon_data;
    Buffer *b = malloc(sizeof(Buffer));

    b->width = (int)(ceil(icon_get_width(icon) / 8.0)*8);
    b->height = icon_get_height(icon);

    uint16_t size = buffer_size(b->width, b->height);
    b->data=malloc_buffer(b->width, b->height);

    CompressIcon* compress_icon = compress_icon_alloc(size);
    compress_icon_decode(compress_icon, icon_get_frame_data(icon, 0), &p_icon_data);

    memcpy(b->data, p_icon_data, size);
    compress_icon_free(compress_icon);

    return b;
}