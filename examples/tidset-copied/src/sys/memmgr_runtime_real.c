#include <stddef.h>
#include <stdlib.h>

#include <lotto/sys/memmgr_runtime.h>

void *
memmgr_runtime_alloc(size_t size)
{
    return malloc(size);
}

void *
memmgr_runtime_aligned_alloc(size_t alignment, size_t size)
{
    void *ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) != 0)
        return NULL;
    return ptr;
}

void *
memmgr_runtime_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void
memmgr_runtime_free(void *ptr)
{
    free(ptr);
}

void
memmgr_runtime_fini(void)
{
}
