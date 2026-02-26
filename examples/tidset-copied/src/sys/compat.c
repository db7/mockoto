#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lotto/sys/abort.h>
#include <lotto/sys/logger.h>
#include <lotto/sys/memmgr_runtime.h>
#include <lotto/sys/stdlib.h>
#include <lotto/sys/string.h>

void *
sys_malloc(size_t n)
{
    return memmgr_runtime_alloc(n);
}

void *
sys_calloc(size_t n, size_t s)
{
    void *p = memmgr_runtime_alloc(n * s);
    if (p != NULL)
        memset(p, 0, n * s);
    return p;
}

void *
sys_realloc(void *p, size_t n)
{
    return memmgr_runtime_realloc(p, n);
}

void
sys_free(void *p)
{
    memmgr_runtime_free(p);
}

void *
sys_memcpy(void *dst, const void *src, size_t n)
{
    return memcpy(dst, src, n);
}

void *
sys_memset(void *dst, int c, size_t n)
{
    return memset(dst, c, n);
}

void
logger_set_level(enum logger_level l)
{
    (void)l;
}

FILE *
logger_fp(void)
{
    return stdout;
}

void
logger(enum logger_level l, FILE *fp)
{
    (void)l;
    (void)fp;
}

static void
vprint_to_stderr(const char *fmt, va_list args)
{
    vfprintf(stderr, fmt, args);
}

void
logger_fatalf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint_to_stderr(fmt, args);
    va_end(args);
    abort();
}

void
logger_errorf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint_to_stderr(fmt, args);
    va_end(args);
}

void
logger_warnf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint_to_stderr(fmt, args);
    va_end(args);
}

void
logger_infof(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint_to_stderr(fmt, args);
    va_end(args);
}

void
logger_debugf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprint_to_stderr(fmt, args);
    va_end(args);
}

void
logger_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void
sys_abort(void)
{
    abort();
}

void
sys_assert_fail(const char *a, const char *file, unsigned int line,
                const char *func)
{
    fprintf(stderr, "assert failed %s(): %s:%u: %s\n", func, file, line, a);
    abort();
}
