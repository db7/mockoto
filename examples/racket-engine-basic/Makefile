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

LIB := libengine_component.$(SHLIB_EXT)

all: $(LIB)

mock/engine.rkt: engine.h
	mkdir -p mock
	$(MOCKOTO) --mode rkt engine.h -- -I. > $@

engine_mock.c: engine.h
	$(MOCKOTO) --mode c engine.h -- -I. > $@

engine_mock.h: engine.h
	$(MOCKOTO) --mode h engine.h -- -I. > $@

$(LIB): engine.c engine_mock.c engine_mock.h
	$(CC) $(CFLAGS) $(SHLIB_FLAG) engine.c engine_mock.c -o $@

test-racket: mock/engine.rkt $(LIB)
	racket test.rkt ./$(LIB)

test: test-racket

clean:
	rm -rf mock engine_mock.c engine_mock.h $(LIB)

.PHONY: all test test-racket clean
