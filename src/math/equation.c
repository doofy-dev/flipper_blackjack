#include "equation.h"
#include "../util/helpers.h"
#include <math.h>

float inverse_tanh(double x)
{
    return 0.5f * (float)log((1 + x) / (1 - x));
}

float lerp_number(float a, float b, float t)
{
    if (t >= 1) return b;
    if (t <= 0) return a;
    return (1 - t) * a + t * b;
}

float smoothstep(float t)
{
    return t * t * (3 - t * 2);
}

float clamp(float x, float lowerlimit, float upperlimit)
{
    if (x < lowerlimit) return lowerlimit;
    if (x > upperlimit) return upperlimit;
    return x;
}

float scale_range(float value, float old_min, float old_max, float new_min, float new_max)
{
    // Avoid division by zero if source range is effectively a single point
    if (old_min == old_max)
    {
        return new_min; // Return the start of the target range
    }

    // Linearly map the value from [x_min, x_max] to [a, b]
    return new_min + (new_max - new_min) * (value - old_min) / (old_max - old_min);
}

int count_digits(uint8_t num)
{
    int size = 1, curr;
    uint8_t n = num;
    while (n > 10)
    {
        curr = n % 10;
        if (curr < 2)
        {
            if (curr > 0)
            {
                size += 3;
            }
            else
            {
                size += 4;
            }
        }
        else
        {
            size += 5;
        }

        size++;

        n /= 10;
    }
    size++;
    return size;
}

/*static const char *SI_FULL_NAMES[23] = {
    "", "thousand", "million", "billion", "trillion",
    "quadrillion", "quintillion", "sextillion", "septillion",
    "octillion", "nonillion", "decillion", "undecillion",
    "duodecillion", "tredecillion", "quattuordecillion",
    "quindecillion", "sexdecillion", "septendecillion",
    "octodecillion", "novemdecillion", "vigintillion"
};*/

static const char* SI_SHORT_NAMES[23] = {
    "", "K", "M", "B", "T",
    "Qa", "Qi", "Sx", "Sp",
    "Oc", "No", "Dc", "Un",
    "Du", "Td", "Qd",
    "Qid", "Sd", "Se",
    "Od", "Nd", "Vg"
};

char* money_formatter(uint32_t score, uint32_t max, uint8_t multiplier)
{
    size_t size = sizeof(char) * 15;
    char* drawChar = allocate(size);

    long double num = score;
    long double mul = multiplier;
    int exponent = 0;
    while (num >= 1000.0 && exponent < 12 - 1)
    {
        num /= 1000.0;

        if (num < 1000 && mul > 0)
        {
            mul--;
            num += max;
        }
        exponent++;
    }

    snprintf(drawChar, size, "$%.2f %s", (double)num, SI_SHORT_NAMES[exponent]);

    return drawChar;
}

void lerp_transform(const PackedTransform* a, const PackedTransform* b, float time, PackedTransform* result)
{
    for (uint8_t i = 0; i < 10; i++)
    {
        result->data[i] = lerp_number(a->data[i], b->data[i], time);
    }
}
