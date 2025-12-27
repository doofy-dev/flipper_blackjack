#pragma once

#include <furi.h>
//#define DEBUG_BUILD

#ifdef DEBUG_BUILD
#define release(X) {_release_debug( X, __FILE__, __LINE__, __FUNCTION__);X=NULL}
#define check_pointer(X) _check_ptr( X, __FILE__, __LINE__, __FUNCTION__)
#define allocate(X) _allocate( X, __FILE__, __LINE__, __FUNCTION__)
#else
#define check_pointer(X) _test_ptr(X)
#define allocate(X) malloc(X)
#define release(X) {_release(X);X=NULL;}
#endif

typedef struct {
    const char* name;
    uint32_t start;
} Timer;

char *get_basename(const char *path);

#ifndef basename
#define basename(path) get_basename(path)
#endif

#define CHECK_HEAP() FURI_LOG_W("Stat", "Free/total heap: %zu / %zu", memmgr_get_free_heap(), memmgr_get_total_heap())

bool _test_ptr(void *p);

bool _check_ptr(void *p, const char *file, int line, const char *func);


size_t curr_time();

void *_allocate(size_t size, const char *file, int line, const char *func);
void _release_debug(void *p,const char *file, int line, const char *func);
void _release(void *p);
void check_leak();


Timer* timer_start(const char*name);
size_t timer_end(Timer *t);