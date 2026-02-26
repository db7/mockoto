#include "sut.h"
#include "runtime.h"

int
make_total(const int *values, unsigned long n)
{
    int *scratch;
    unsigned long i;
    int total;

    scratch = (int *)memmgr_runtime_alloc(n * sizeof(int));
    if (!scratch)
        return -1;

    for (i = 0; i < n; i++)
        scratch[i] = values[i];

    total = 0;
    for (i = 0; i < n; i++)
        total += scratch[i];

    memmgr_runtime_free(scratch);
    return total;
}
