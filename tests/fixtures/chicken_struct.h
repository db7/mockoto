#ifndef MOCKOTO_TEST_CHICKEN_STRUCT_H
#define MOCKOTO_TEST_CHICKEN_STRUCT_H

typedef unsigned char uint8_t;

enum chicken_kind {
    CHICKEN_KIND_NONE = 0,
    CHICKEN_KIND_KEY = 1,
};

struct chicken_intent {
    enum chicken_kind kind;
    _Bool press;
    union {
        uint8_t keycode;
        int raw;
    } v;
};

void chicken_touch(struct chicken_intent *in);

#endif
