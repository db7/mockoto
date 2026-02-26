# Mockoto Examples

These examples are self-contained and use simple `Makefile` workflows.

## Layout

- `pure-c-runtime-hooks`: Pure C test using generated `--mode c` / `--mode h` mocks.
  This mirrors lotto-style unit tests that call `mockoto_<fn>_hook` in C.
- `racket-engine-basic`: Racket test using generated `--mode rkt` bindings with
  `load-lib`, `call`, `mock`, and `cfunc`.
- `racket-dual-bindings`: Racket test using two generated bindings, resembling
  lotto tests that combine multiple binding files.
- `racket-stateful-a-mock-b`: Stateful `A` tested from Racket while mocking
  stateful dependency `B`.
- `tidset-copied`: Copied `tidset` (`0BSD`) and corresponding C/Racket tests in
  one self-contained example.

## Prerequisites

- `mockoto` in `PATH`, or pass `MOCKOTO=/path/to/mockoto`
- `cc`
- For Racket examples: `racket`

## Quick start

```sh
cd mockoto/examples
make test-all
```

If `mockoto` is not in `../../build/mockoto`, pass it explicitly:

```sh
cd mockoto/examples
make test-all MOCKOTO=/path/to/mockoto
```
