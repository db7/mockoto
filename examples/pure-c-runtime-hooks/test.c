#include <assert.h>

#include "runtime_mock.h"
#include "sut.h"

static int fake_buf[16];
static unsigned long seen_alloc_size;
static void *seen_free_ptr;

static void *
fake_alloc(unsigned long nbytes)
{
    seen_alloc_size = nbytes;
    return fake_buf;
}

static void
fake_free(void *ptr)
{
    seen_free_ptr = ptr;
}

int
main(void)
{
    int values[] = {1, 2, 3, 4};
    int total;

    mockoto_memmgr_runtime_alloc_hook(fake_alloc);
    mockoto_memmgr_runtime_free_hook(fake_free);

    total = make_total(values, 4);
    assert(total == 10);

    assert(seen_alloc_size == 4 * sizeof(int));
    assert(seen_free_ptr == fake_buf);
    assert(mockoto_memmgr_runtime_alloc_called() == 1);
    assert(mockoto_memmgr_runtime_free_called() == 1);

    mockoto_memmgr_runtime_alloc_hook(0);
    mockoto_memmgr_runtime_free_hook(0);
    mockoto_memmgr_runtime_alloc_returns(fake_buf);
    total = make_total(values, 2);
    assert(total == 3);
    assert(mockoto_memmgr_runtime_alloc_called() == 1);
    assert(mockoto_memmgr_runtime_free_called() == 1);

    return 0;
}
