MOCKOTO ?= mockoto
CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -O0 -g

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
SHLIB_EXT := dylib
SHLIB_FLAG := -dynamiclib
else
SHLIB_EXT := so
SHLIB_FLAG := -shared
endif

LIB := liba_component.$(SHLIB_EXT)

all: $(LIB)

mock/a.rkt: a.h b.h
	mkdir -p mock
	$(MOCKOTO) --mode rkt a.h b.h -- -I. > $@

mock/b.rkt: b.h
	mkdir -p mock
	$(MOCKOTO) --mode rkt b.h -- -I. > $@

b_mock.c: b.h
	$(MOCKOTO) --mode c b.h -- -I. > $@

b_mock.h: b.h
	$(MOCKOTO) --mode h b.h -- -I. > $@

$(LIB): a.c b_mock.c b_mock.h a.h
	$(CC) $(CFLAGS) $(SHLIB_FLAG) a.c b_mock.c -o $@

test-racket: mock/a.rkt mock/b.rkt $(LIB)
	racket test.rkt ./$(LIB)

test: test-racket

clean:
	rm -rf mock b_mock.c b_mock.h $(LIB)

.PHONY: all test test-racket clean
