# racket-dual-bindings

Racket example resembling lotto tests that combine multiple generated bindings.

## What it shows

- Generate `mock/engine.rkt` and `mock/trace.rkt`
- Import binding APIs with `only-in` and renaming
- Load the same shared library for each binding module
- Use one module for SUT calls and another for mock hooks

## Run

```sh
make test
```

If `mockoto` is not in `PATH`:

```sh
make test MOCKOTO=/path/to/mockoto
```
