# tidset-copied

Copied `tidset` component (`0BSD`) and its corresponding tests in one self-contained example.

## Included tests

- `test-c`: C test adapted from `test/unit/base/tidset_test.c` using generated
  `memmgr_runtime` mock hooks.
- `test-racket`: Racket test adapted from `test/racket/base/tidset_test.rkt`
  using generated `tidset` bindings.
- `test-chibi`: Chibi Scheme test using generated `--mode chibi` bindings.
- `test-chicken`: CHICKEN Scheme test using generated `--mode chicken`
  bindings.

## Run

```sh
make test
make test-chibi
make test-chicken
```

If `mockoto` is not in `PATH`:

```sh
make test MOCKOTO=/path/to/mockoto
make test-chibi MOCKOTO=/path/to/mockoto
make test-chicken MOCKOTO=/path/to/mockoto
```
