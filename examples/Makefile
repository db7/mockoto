MOCKOTO ?= ../../build/mockoto

EXAMPLES := \
	pure-c-runtime-hooks \
	racket-engine-basic \
	racket-dual-bindings \
	racket-stateful-a-mock-b \
	tidset-copied

all: test-all

test-all:
	@for d in $(EXAMPLES); do \
		echo "==> $$d"; \
		$(MAKE) -C $$d test MOCKOTO=$(MOCKOTO); \
	done

clean-all:
	@for d in $(EXAMPLES); do \
		echo "==> clean $$d"; \
		$(MAKE) -C $$d clean; \
	done

.PHONY: all test-all clean-all
