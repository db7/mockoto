#ifndef MOCKOTO_TEST_BASIC_H
#define MOCKOTO_TEST_BASIC_H

int add(int a, int b);
void ping(void);
char *fetch_name(int id);
int apply_cb(int (*cb)(int), int value);
int zero(void);

#endif
