#ifndef LOTTO_TIDSET_TEST_API_H
#define LOTTO_TIDSET_TEST_API_H

#include <lotto/base/tidset.h>

struct tidset *tidset_new(void);
void tidset_delete(struct tidset *tset);

void tidset_sort_lt_apply(struct tidset *tset);
void tidset_sort_gt_apply(struct tidset *tset);
void tidset_filter_not_multiple_of_4(struct tidset *tset);

#endif
