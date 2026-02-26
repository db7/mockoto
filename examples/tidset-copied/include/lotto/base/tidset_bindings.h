#ifndef LOTTO_TIDSET_BINDINGS_H
#define LOTTO_TIDSET_BINDINGS_H

#include <lotto/base/tidset.h>
#include <lotto/base/tidset_test_api.h>

size_t tidset_size(const tidset_t *tset);
_Bool tidset_insert(tidset_t *tset, task_id id);
_Bool tidset_has(const tidset_t *tset, task_id id);
task_id tidset_get(const tidset_t *tset, size_t idx);
void tidset_intersect(tidset_t *tset1, const tidset_t *tset2);
void tidset_subtract(tidset_t *tset1, const tidset_t *tset2);
_Bool tidset_contains_all(const tidset_t *tset1, const tidset_t *tset2);

struct tidset *tidset_new(void);
void tidset_delete(struct tidset *tset);
void tidset_sort_lt_apply(struct tidset *tset);
void tidset_sort_gt_apply(struct tidset *tset);
void tidset_filter_not_multiple_of_4(struct tidset *tset);

#endif
