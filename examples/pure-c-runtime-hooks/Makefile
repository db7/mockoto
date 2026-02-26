MOCKOTO ?= mockoto
CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -O0 -g

all: test

test: pure_c_test
	./pure_c_test

runtime_mock.c: runtime.h
	$(MOCKOTO) --mode c runtime.h -- -I. > $@

runtime_mock.h: runtime.h
	$(MOCKOTO) --mode h runtime.h -- -I. > $@

pure_c_test: sut.c test.c runtime_mock.c runtime_mock.h sut.h
	$(CC) $(CFLAGS) -I. sut.c test.c runtime_mock.c -o $@

clean:
	rm -f runtime_mock.c runtime_mock.h pure_c_test

.PHONY: all test clean
