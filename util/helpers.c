#include "helpers.h"
#include "list.h"
#include <furi.h>
#include <math.h>

int16_t pointer_count=0;

float inverse_tanh(double x) {
    return 0.5f * (float) log((1 + x) / (1 - x));
}

float lerp_number(float a, float b, float t) {
    if (t >= 1) return b;
    if (t <= 0) return a;
    return (1 - t) * a + t * b;
}

bool _test_ptr(void *p) {
    return p != NULL;
}

bool _check_ptr(void *p, const char *file, int line, const char *func) {
    UNUSED(file);
    UNUSED(line);
    UNUSED(func);
    if (p == NULL) {
        FURI_LOG_W("App", "[NULLPTR] %s:%s():%i", get_basename((char *) file), func, line);
    }

    return _test_ptr(p);
}

char *get_basename(const char *path) {
    const char *base = path;
    while (*path) {
        if (*path++ == '/') {
            base = path;
        }
    }
    return (char *) base;
}

size_t curr_time() { return DWT->CYCCNT; }


/*static const char *SI_FULL_NAMES[23] = {
    "", "thousand", "million", "billion", "trillion",
    "quadrillion", "quintillion", "sextillion", "septillion",
    "octillion", "nonillion", "decillion", "undecillion",
    "duodecillion", "tredecillion", "quattuordecillion",
    "quindecillion", "sexdecillion", "septendecillion",
    "octodecillion", "novemdecillion", "vigintillion"
};*/

static const char *SI_SHORT_NAMES[23] = {
    "", "K", "M", "B", "T",
    "Qa", "Qi", "Sx", "Sp",
    "Oc", "No", "Dc", "Un",
    "Du", "Td", "Qd",
    "Qid", "Sd", "Se",
    "Od", "Nd", "Vg"
};

char *money_formatter(uint32_t score, uint32_t max, uint8_t multiplier) {
    size_t size = sizeof(char) * 15;
    char *drawChar = allocate(size);

    long double num = score;
    long double mul = multiplier;
    int exponent = 0;
    while (num >= 1000.0 && exponent < 12 - 1) {
        num /= 1000.0;

        if (num < 1000 && mul > 0) {
            mul--;
            num += max;
        }
        exponent++;
    }

    snprintf(drawChar, size, "$%.2f %s", (double) num, SI_SHORT_NAMES[exponent]);

    return drawChar;
}

int count_digits(uint8_t num) {
    int size = 1, curr;
    uint8_t n = num;
    while (n > 10) {
        curr = n % 10;
        if (curr < 2) {
            if (curr > 0) {
                size += 3;
            } else {
                size += 4;
            }
        } else {
            size += 5;
        }

        size++;

        n /= 10;
    }
    size++;
    return size;
}

void *_allocate(size_t size, const char *file, int line, const char *func) {
    pointer_count++;
    void *data=malloc(size);
    FURI_LOG_D("Memory", "Allocated %d bytes at %p\tcaller: %s:%s():%i", size, data, get_basename((char *) file), func, line);
    return data;
}

void _release_debug(void *p, const char *file, int line, const char *func) {
    check_pointer(p);
    pointer_count--;
    FURI_LOG_D("Memory", "Releasing %p\tcaller:%s:%s():%i", p, get_basename((char *) file), func, line);

    _release(p);
}

void _release(void *p){
    free(p);
    p=NULL;
}

void check_leak(){
    if(pointer_count>0){
        FURI_LOG_E("Memory", "Leak detected, pointers left in memory: %d", pointer_count);
    }
}