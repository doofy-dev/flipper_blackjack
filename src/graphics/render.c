#include "render.h"

#include <float.h>

#include "../math/equation.h"
#include "../util/helpers.h"

#define EPSILON 1e-6f

static Matrix* current_transform;
static Matrix identity_transform = IDENTITY_MATRIX;
static Vector camera_position = {0, 0};
static CULL_MODE cull_mode = CULL_BACK;

void set_cull_mode(CULL_MODE mode)
{
    cull_mode = mode;
}

void set_camera(Vector position)
{
    camera_position = position;
}

Vector get_camera()
{
    return camera_position;
}

void set_transform(Matrix* transform)
{
    if (transform)
    {
        current_transform = transform;
    }
    else
    {
        current_transform = &identity_transform;
    }
}

void render_rbox(Buffer *screen, RenderData *data, Vector *scaling, Vector *pixel, Vector *uv)
{
    float w = scaling->x;
    float h = scaling->y;
    float r = 0.15f; // corner radius
    float thickness = 0.05f; // edge thickness

    // Remap UV to [-size/2, size/2]
    float dx = ABS(uv->x - 0.5f) * w;
    float dy = ABS(uv->y - 0.5f) * h;

    // Distance to the inner boundary where rounding starts
    float qx = dx - (w * 0.5f - r);
    float qy = dy - (h * 0.5f - r);

    // Calculate distance to the edge
    float dist = sqrtf(fmaxf(qx, 0.0f) * fmaxf(qx, 0.0f) + fmaxf(qy, 0.0f) * fmaxf(qy, 0.0f))
                 + fminf(fmaxf(qx, qy), 0.0f) - r;

    // If dist < 0, we are inside the rounded rect
    if (dist <= 0.0f) {
        if (dist > -thickness)
            buffer_set_pixel(screen, pixel->x, pixel->y, data->color);
        else if (data->mask_color != COLOR_TRANSPARENT)
            buffer_set_pixel(screen, pixel->x, pixel->y, data->mask_color);
    }
}

void render_uv(Buffer* screen, RenderData* data, Vector* scaling, Vector* pixel, Vector* uv)
{
    // Factors for tiling if applicable (we assume sprite->width and sprite->height are constant)

    if (data->tile_mode != TILE_NONE)
    {
        const float inv_width = 1.0f / data->sprite->width;
        const float inv_height = 1.0f / data->sprite->height;
        const float max_u = 1.0f - inv_width;
        const float max_v = 1.0f - inv_height;
        float u = uv->x * scaling->x; // Scale UV X
        float v = uv->y * scaling->y; // Scale UV Y

        // Horizontal tiling
        if (data->tile_mode & TILE_HORIZONTAL)
        {
            u -= (float)((int)u); // Wrap to [0, 1)
            u = (u < 0.0f) ? (u + 1.0f) : u; // Handle negative values
            u = fminf(u, max_u); // Clamp near-right edges to avoid out-of-bounds sampling
        }

        // Vertical tiling
        if (data->tile_mode & TILE_VERTICAL)
        {
            v -= (float)((int)v); // Wrap to [0, 1)
            v = (v < 0.0f) ? (v + 1.0f) : v; // Handle negative values
            v = fminf(v, max_v); // Clamp near-bottom edges to avoid out-of-bounds sampling
        }

        uv->x = u;
        uv->y = v;
    }
    else if (uv->x < 0 || uv->x > 1 || uv->y < 0 || uv->y > 1)
    {
        return;
    }

    // Sample the texture and render the pixel
    if (data->mask && buffer_sample(data->mask, uv))
    {
        buffer_set_pixel(screen, pixel->x, pixel->y, data->mask_color);
    }
    if (buffer_sample(data->sprite, uv))
    {
        buffer_set_pixel(screen, pixel->x, pixel->y, data->color);
    }
}

void render_filled(Buffer* screen, RenderData* data, Vector* scaling, Vector* pixel, Vector* uv)
{
    UNUSED(scaling);
    UNUSED(data);
    UNUSED(uv);
    buffer_set_pixel(screen, pixel->x, pixel->y, data->color);
}

void flip_uv(Poly* poly, FlipMode flip_mode)
{
    if (flip_mode)
    {
        for (uint8_t i = 0; i < 4; ++i)
        {
            if (flip_mode & FLIP_HORIZONTAL)
                poly->uv[i].x = 1 - poly->uv[i].x;
            if (flip_mode & FLIP_VERTICAL)
                poly->uv[i].y = 1 - poly->uv[i].y;
        }
    }
}

static float edge_function(Vector* const a, Vector* const b, Vector* const c)
{
    return (c->x - a->x) * (b->y - a->y) - (c->y - a->y) * (b->x - a->x);
}

void rasterize_triangle(Buffer* buffer, RenderData* data, Vector* scaling,
                        Vector* const A, Vector* const B, Vector* const C,
                        Vector* const uvA, Vector* const uvB, Vector* const uvC)
{
    // Precompute the full triangle area (for barycentric coordinates)
    float area = edge_function(A, B, C);

    //apply culling
    if (cull_mode == CULL_FRONT && area >= EPSILON) return;
    if (cull_mode == CULL_BACK && area <= EPSILON) return;

    // Compute the bounding box of the triangle
    uint8_t minX = MAX(0, FLOOR(MIN(MIN(A->x, B->x), C->x)));
    uint8_t minY = MAX(0, FLOOR(MIN(MIN(A->y, B->y), C->y)));
    uint8_t maxX = MIN(SCREEN_WIDTH - 1, CEIL(MAX(MAX(A->x, B->x), C->x)));
    uint8_t maxY = MIN(SCREEN_HEIGHT - 1, CEIL(MAX(MAX(A->y, B->y), C->y)));

    Vector pixel, uv;

    Vector P0 = {minX + 0.5f, minY + 0.5f};

    // Initial barycentric weights
    float w0_row = edge_function(B, C, &P0) / area;
    float w1_row = edge_function(C, A, &P0) / area;
    float w2_row = edge_function(A, B, &P0) / area;

    // Gradient of barycentric weights along x and y directions
    float w0_dx = (C->y - B->y) / area; // Partial derivative w.r.t. x
    float w1_dx = (A->y - C->y) / area;
    float w2_dx = (B->y - A->y) / area;

    float w0_dy = (B->x - C->x) / area; // Partial derivative w.r.t. y
    float w1_dy = (C->x - A->x) / area;
    float w2_dy = (A->x - B->x) / area;
    float w0, w1, w2;

    // Rasterize the triangle
    for (uint8_t y = minY; y <= maxY; y++)
    {
        pixel.y = y;
        // Start barycentric weights for this row
        w0 = w0_row;
        w1 = w1_row;
        w2 = w2_row;

        for (uint8_t x = minX; x <= maxX; x++)
        {
            pixel.x = x;

            // Check if the point is inside the triangle
            if ((w0 >= 0) & (w1 >= 0) & (w2 >= 0))
            {
                // Interpolate UV coordinates using barycentric weights
                uv.x = w0 * uvA->x + w1 * uvB->x + w2 * uvC->x;
                uv.y = w0 * uvA->y + w1 * uvB->y + w2 * uvC->y;
                data->callback(buffer, data, scaling, &pixel, &uv);
            }

            // Increment weights horizontally
            w0 += w0_dx;
            w1 += w1_dx;
            w2 += w2_dx;
        }

        // Increment weights vertically
        w0_row += w0_dy;
        w1_row += w1_dy;
        w2_row += w2_dy;
    }
}

void cache_position(Poly* poly, Transform *transform)
{

    if (!transform->dirty) return;

    for (uint8_t i = 0; i < 4; i++)
    {
        matrix_mul_vector(&transform->transformation_matrix, &poly->corners[i], &poly->cached_corners[i]);
    }
    transform->dirty = false;
}

void rasterize(Buffer* buffer, RenderData* data)
{
    if (!buffer || !data) return;
    // Timer *timer = timer_start("rasterize start");

    if (!data->callback)
    {
        data->callback = data->sprite ? render_uv : render_filled;
    }
    // timer_end(timer);

    // timer = timer_start("cull");

    Vector corner[4];

    float x_min = FLT_MAX, x_max = -FLT_MAX, y_min = FLT_MAX, y_max = -FLT_MAX;

    bool within_screen = false; // To determine if any corner is inside the screen

    // frustum cull
    for (int i = 0; i < 4; i++)
    {
        corner[i] = (Vector){
            data->poly.cached_corners[i].x - camera_position.x, data->poly.cached_corners[i].y - camera_position.y
        };
        x_min = MIN(corner[i].x, corner[(i + 1) % 4].x);
        x_max = MAX(corner[i].x, corner[(i + 1) % 4].x);
        y_min = MIN(corner[i].y, corner[(i + 1) % 4].y);
        y_max = MAX(corner[i].y, corner[(i + 1) % 4].y);
        if (corner[i].x >= 0 && corner[i].x < SCREEN_WIDTH &&
            corner[i].y >= 0 && corner[i].y < SCREEN_HEIGHT)
        {
            within_screen = true;
        }
    }

    // timer_end(timer);
    // Check if the AABB overlaps the screen
    if (!within_screen && (x_min < 0 && x_max >= SCREEN_WIDTH && y_min < 0 && y_max >= SCREEN_HEIGHT))
    {
        // Sprite is fully outside the screen bounds
        return;
    }

    Vector scale = {
        1, 1,
        // data->sprite->width / MAX(x_max-x_min, 1),
        // data->sprite->height / MAX(y_min-y_max, 1)
    };

    // timer = timer_start("raster");
    rasterize_triangle(buffer, data, &scale,
                       &(corner[0]), &(corner[2]), &(corner[1]),
                       &(data->poly.uv[0]), &(data->poly.uv[2]), &(data->poly.uv[1])
    );
    rasterize_triangle(buffer, data, &scale,
                       &(corner[0]), &(corner[3]), &(corner[2]),
                       &(data->poly.uv[0]), &(data->poly.uv[3]), &(data->poly.uv[2])
    );


    // timer_end(timer);
}

void draw_line(Buffer* buffer, Vector* a, Vector* b, PixelColor render_color)
{
    Vector _a, _b;
    if (current_transform)
    {
        matrix_mul_vector(current_transform, a, &_a);
        matrix_mul_vector(current_transform, b, &_b);
    }else
    {
        _a = *a;
        _b = *b;
    }
    int16_t _x0 = (int16_t)_a.x - camera_position.x;
    int16_t _x1 = (int16_t)_b.x - camera_position.x;
    int16_t _y0 = (int16_t)_a.y - camera_position.y;
    int16_t _y1 = (int16_t)_b.y - camera_position.y;
    int dx = abs(_x1 - _x0);
    int16_t sx = _x0 < _x1 ? 1 : -1;

    int dy = abs(_y1 - _y0);
    int16_t sy = _y0 < _y1 ? 1 : -1;

    int err = (dx > dy ? dx : -dy) / 2;

    while (true)
    {
        if (buffer_test_coordinate(buffer, _x0, _y0))
        {
            buffer_set_pixel(buffer, _x0, _y0, render_color);
        }
        if (_x0 == _x1 && _y0 == _y1) break;
        int e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            _x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            _y0 += sy;
        }
    }
}

void draw_rbox(Buffer* buffer, const Rect rect, PixelColor color, PixelColor fill_color)
{
    //draw rectangle lines, leave 1 pixel out for corners
    draw_line(buffer, &(Vector){rect.x + 1, rect.y}, &(Vector){rect.x + rect.width - 1, rect.y}, color); //top
    draw_line(buffer, &(Vector){rect.x, rect.y + 1}, &(Vector){rect.x, rect.y + rect.height - 1}, color); //left
    draw_line(buffer, &(Vector){rect.x + rect.width, rect.y + 1},
              &(Vector){rect.x + rect.width, rect.y + rect.height - 1}, color); //right
    draw_line(buffer, &(Vector){rect.x + 1, rect.y + rect.height},
              &(Vector){rect.x + rect.width - 1, rect.y + rect.height}, color); //bottom


    if (fill_color != COLOR_TRANSPARENT)
    {
        for (int16_t i = (int16_t)rect.x + 1; i < (int16_t)(rect.x + rect.width) - 1; i++)
        {
            for (int16_t j = (int16_t)rect.y + 1; j < (int16_t)(rect.y + rect.height) - 1; j++)
            {
                buffer_set_pixel(buffer, i, j, fill_color);
            }
        }
    }
}
