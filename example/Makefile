MOCKOTO?=../build/mockoto
CC=clang
CFLAGS=-Wall -Werror

build: example_test.bin

racket: example.rkt libexample.so

chibi-test:
	MOCKOTO="$(MOCKOTO)" ./chibi_test.sh

%_mock.c: %.h
	$(MOCKOTO) --mode c $< -- -I. > $@

%_mock.h: %.h
	$(MOCKOTO) --mode h $< -- -I. > $@

%.rkt: client.h server.h
	$(MOCKOTO) --mode rkt $^ -- -I. > $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

example_test.bin: client.o server_mock.o example_test.o
	$(CC) $(CFLAGS) -o $@ $^

libexample.so: client.o server_mock.o
	$(CC) $(CFLAGS) -shared -o $@ $^

clean:
	rm -rf *.o *.so *.bin *_mock.* example.rkt example.stub

.PHONY: build clean racket chibi-test
.SECONDARY: server_mock.c server_mock.h
