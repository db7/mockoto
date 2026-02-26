#ifndef EXAMPLE_A_H
#define EXAMPLE_A_H

#include "b.h"

typedef struct a_state {
    int counter;
    int last;
} a_state_t;

void a_init(a_state_t *st, int seed);
int a_step(a_state_t *st, int input);
int a_total(a_state_t *st, int probe);

#endif
