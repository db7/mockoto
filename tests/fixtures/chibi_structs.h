#ifndef MOCKOTO_TEST_CHIBI_STRUCTS_H
#define MOCKOTO_TEST_CHIBI_STRUCTS_H

#include <stdint.h>

enum sample_status {
  SAMPLE_MORE = 1,
  SAMPLE_DONE = 2,
};

struct sample_report {
  int8_t x;
  int8_t y;
  uint8_t scroll;
};

void sample_reset(struct sample_report *report);

#endif
