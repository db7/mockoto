MOCKOTO ?= mockoto
CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -O0 -g
CHIBI_CFLAGS ?= -std=c99 -Wall -Wextra -O0 -g -Wno-unused-parameter
CPPFLAGS := -Iinclude -I.
CHIBI_SCHEME ?= chibi-scheme
CHIBI_FFI ?= chibi-ffi
CHICKEN ?= csi
CHICKENC ?= csc
CHICKEN_CPPFLAGS ?= -Iinclude -I/Users/diogo/.local/include

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
SHLIB_EXT := dylib
SHLIB_FLAG := -dynamiclib
else
SHLIB_EXT := so
SHLIB_FLAG := -shared
endif

LIB := libtidset_component.$(SHLIB_EXT)
CHIBI_LIB := libtidset_chibi.$(SHLIB_EXT)
CHICKEN_TEST_BIN := tidset_chicken_test

all: $(LIB)

mock/tidset.rkt: include/lotto/base/task_id.h include/lotto/base/marshable.h include/lotto/base/tidmap.h include/lotto/base/tidset.h include/lotto/base/tidset_test_api.h
	mkdir -p mock
	$(MOCKOTO) --mode rkt \
		include/lotto/base/task_id.h \
		include/lotto/base/marshable.h \
		include/lotto/base/tidmap.h \
		include/lotto/base/tidset.h \
		include/lotto/base/tidset_test_api.h -- -Iinclude > $@

mock/tidset.stub: include/lotto/base/tidset_bindings.h
	mkdir -p mock
	$(MOCKOTO) --mode chibi include/lotto/base/tidset_bindings.h -- -Iinclude > $@

mock/tidset.chibi.c: mock/tidset.stub
	$(CHIBI_FFI) $< $@ >/dev/null

mock/tidset.chicken.scm: include/lotto/base/tidset_bindings.h
	mkdir -p mock
	$(MOCKOTO) --mode chicken include/lotto/base/tidset_bindings.h -- -Iinclude > $@

memmgr_runtime_mock.c: include/lotto/sys/memmgr_runtime.h
	$(MOCKOTO) --mode c include/lotto/sys/memmgr_runtime.h -- -Iinclude > $@

memmgr_runtime_mock.h: include/lotto/sys/memmgr_runtime.h
	$(MOCKOTO) --mode h include/lotto/sys/memmgr_runtime.h -- -Iinclude > $@

tidset_c_test: test/tidset_test.c src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c memmgr_runtime_mock.c memmgr_runtime_mock.h
	$(CC) $(CFLAGS) $(CPPFLAGS) test/tidset_test.c src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c memmgr_runtime_mock.c -o $@

$(LIB): src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c src/sys/memmgr_runtime_real.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SHLIB_FLAG) src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c src/sys/memmgr_runtime_real.c -o $@

$(CHIBI_LIB): mock/tidset.chibi.c src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c src/sys/memmgr_runtime_real.c
	$(CC) $(CHIBI_CFLAGS) $(CPPFLAGS) $(SHLIB_FLAG) \
		-I/usr/local/pkg/include \
		-L/usr/local/pkg/lib \
		mock/tidset.chibi.c \
		src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c src/sys/memmgr_runtime_real.c \
		-lchibi-scheme \
		-o $@

$(CHICKEN_TEST_BIN): test/tidset_test.chicken.scm mock/tidset.chicken.scm src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c src/sys/memmgr_runtime_real.c
	$(CHICKENC) -O2 $(CHICKEN_CPPFLAGS) -o $@ test/tidset_test.chicken.scm src/base/tidset.c src/base/tidset_test_api.c src/base/tidmap_stub.c src/sys/compat.c src/sys/memmgr_runtime_real.c

test-c: tidset_c_test
	./tidset_c_test

test-racket: mock/tidset.rkt $(LIB)
	racket test/tidset_test.rkt ./$(LIB)

test-chibi: $(CHIBI_LIB)
	$(CHIBI_SCHEME) test/tidset_test.chibi.scm ./$(CHIBI_LIB)

test-chicken: $(CHICKEN_TEST_BIN)
	./$(CHICKEN_TEST_BIN)

test: test-c test-racket
test-all: test-c test-racket test-chibi test-chicken

clean:
	rm -rf mock memmgr_runtime_mock.c memmgr_runtime_mock.h tidset_c_test $(LIB) $(CHIBI_LIB) $(CHICKEN_TEST_BIN) *.dSYM

.PHONY: all test test-all test-c test-racket test-chibi test-chicken clean
