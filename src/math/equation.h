#pragma once
#include <furi.h>

#include "vector.h"

#define M_PIX2        6.28318530717958647692    /* 2 pi */
#define FABS(x) ((x) < 0 ? -(x) : (x))
#define DEG_2_RAD  0.01745329251994329576f
#define RAD_2_DEG  565.48667764616278292327f

#define FLOOR(x) (floorf(x))
#define CEIL(x) ((ceilf(x)))

#define deg2rad(deg) ((deg) * DEG_2_RAD)
#define rad2deg(rad) ((rad) * RAD_2_DEG)

typedef union PackedTransform
{
    struct
    {
        Vector position;
        Vector scale;
        float rotation;
    };
    float data[10];
} PackedTransform;

float inverse_tanh(double x);

float lerp_number(float a, float b, float t);
float smoothstep(float t);
float clamp(float x, float lowerlimit, float upperlimit);
float scale_range(float value, float old_min, float old_max, float new_min, float new_max);
int count_digits(uint8_t num);
char* money_formatter(uint32_t score, uint32_t max, uint8_t multiplier);

void lerp_transform(const PackedTransform * a, const PackedTransform * b, float time, PackedTransform *result);