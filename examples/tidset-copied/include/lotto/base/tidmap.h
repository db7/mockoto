#ifndef LOTTO_TIDMAP_H
#define LOTTO_TIDMAP_H

#include <lotto/base/task_id.h>

typedef struct tiditem {
    task_id id;
} tiditem_t;

typedef struct tidmap {
    unsigned char _unused;
} tidmap_t;

tiditem_t *tidmap_find(const tidmap_t *map, task_id id);

#endif
