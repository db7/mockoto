#include "a.h"

void
a_init(a_state_t *st, int seed)
{
    st->counter = seed;
    st->last = 0;
}

int
a_step(a_state_t *st, int input)
{
    int out = b_transform(st->counter, input);
    st->counter += 1;
    st->last = out;
    return out;
}

int
a_total(a_state_t *st, int probe)
{
    return st->last + probe;
}
