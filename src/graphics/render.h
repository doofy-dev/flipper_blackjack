#pragma once
#include "../math/matrix.h"
#include "../math/vector.h"
#include "buffer.h"

typedef struct RenderData RenderData;

typedef struct {
    float x, y, width, height;
} Rect;

typedef enum
{
    CULL_NONE,
    CULL_BACK,
    CULL_FRONT
} CULL_MODE;

typedef enum {
    FLIP_NONE,
    FLIP_HORIZONTAL,
    FLIP_VERTICAL
} FlipMode;

typedef enum {
    TILE_NONE = 0, // 0b0000 (no tiling)
    TILE_HORIZONTAL = 1 << 0, // 0b0001 (bit 0 set)
    TILE_VERTICAL = 1 << 1, // 0b0010 (bit 1 set)
    TILE_BOTH = TILE_HORIZONTAL | TILE_VERTICAL // 0b0011 (combination of horizontal and vertical)
} TileMode;

typedef struct {
    Vector corners[4];
    Vector cached_corners[4];
    Vector uv[4];
} Poly;


#define RECTANGLE(x,y, width, height) (Poly){\
.corners = { {x, y}, {x + width, y}, {x + width, y + height}, {x, y + height} },\
.cached_corners={{x, y}, {x + width, y}, {x + width, y + height}, {x, y + height}},\
.uv = {{0, 0},{1, 0},{1, 1},{0, 1}}\
}

struct RenderData {
    Poly poly;
    Buffer *sprite;
    Buffer *mask;
    PixelColor color;
    PixelColor mask_color;
    TileMode tile_mode;

    void (*callback)(Buffer *screen, RenderData *data, Vector *scaling, Vector *pixel, Vector *uv);
};

void set_camera(Vector position);

Vector get_camera();

void render_rbox(Buffer *screen, RenderData *data, Vector *scaling, Vector *pixel, Vector *uv);
void render_uv(Buffer *screen, RenderData *data, Vector *scaling, Vector *pixel, Vector *uv);
void render_filled(Buffer *screen, RenderData *data, Vector *scaling, Vector *pixel, Vector *uv);

void set_transform(Matrix *transform);
void set_cull_mode(CULL_MODE mode);

void set_pixel(Buffer *buffer, Vector *screen);

void set_color(PixelColor color);

void rasterize(Buffer *buffer, RenderData *data);

void draw_line(Buffer *buffer, Vector *a, Vector *b, PixelColor color);

void draw_rbox(Buffer *buffer, Rect rect, PixelColor color, PixelColor fill_color);

void flip_uv(Poly *poly, FlipMode flip_mode);

void cache_position(Poly *poly, Transform *transform);