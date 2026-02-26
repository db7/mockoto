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

LIB := libpipeline_component.$(SHLIB_EXT)

all: $(LIB)

mock/engine.rkt: engine.h
	mkdir -p mock
	$(MOCKOTO) --mode rkt engine.h -- -I. > $@

mock/trace.rkt: trace.h
	mkdir -p mock
	$(MOCKOTO) --mode rkt trace.h -- -I. > $@

trace_mock.c: trace.h
	$(MOCKOTO) --mode c trace.h -- -I. > $@

trace_mock.h: trace.h
	$(MOCKOTO) --mode h trace.h -- -I. > $@

$(LIB): pipeline.c trace_mock.c trace_mock.h
	$(CC) $(CFLAGS) $(SHLIB_FLAG) pipeline.c trace_mock.c -o $@

test-racket: mock/engine.rkt mock/trace.rkt $(LIB)
	racket test.rkt ./$(LIB)

test: test-racket

clean:
	rm -rf mock trace_mock.c trace_mock.h $(LIB)

.PHONY: all test test-racket clean
