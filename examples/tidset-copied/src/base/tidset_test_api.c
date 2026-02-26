#include <lotto/base/tidset_test_api.h>
#include <lotto/sys/stdlib.h>

struct tidset *
tidset_new(void)
{
    struct tidset *tset = (struct tidset *)sys_malloc(sizeof(struct tidset));
    if (tset != (void *)0)
        tidset_init(tset);
    return tset;
}

void
tidset_delete(struct tidset *tset)
{
    if (tset == (void *)0)
        return;
    tidset_fini(tset);
    sys_free(tset);
}

void
tidset_sort_lt_apply(struct tidset *tset)
{
    tidset_sort(tset, tidset_cmp_lt);
}

void
tidset_sort_gt_apply(struct tidset *tset)
{
    tidset_sort(tset, tidset_cmp_gt);
}

static _Bool
keep_not_multiple_of_4(task_id v)
{
    return (v % 4) != 0;
}

void
tidset_filter_not_multiple_of_4(struct tidset *tset)
{
    tidset_filter(tset, keep_not_multiple_of_4);
}
