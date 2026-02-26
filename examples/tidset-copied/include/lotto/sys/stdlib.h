#ifndef LOTTO_STDLIB_H
#define LOTTO_STDLIB_H

#include <stddef.h>

void *sys_malloc(size_t n);
void *sys_calloc(size_t n, size_t s);
void *sys_realloc(void *p, size_t n);
void sys_free(void *p);

#endif
