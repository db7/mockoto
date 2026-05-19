#pragma once

enum color {
  COLOR_RED = 1,
  COLOR_BLUE = 2,
};

struct item {
  int value;
};

union payload {
  int i;
  const char *s;
};

struct tagged_fields {
  enum color color;
  struct item *item;
  union payload *payload;
};
