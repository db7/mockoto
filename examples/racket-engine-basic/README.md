# racket-engine-basic

Racket example resembling lotto Racket tests that use one generated binding file.

## What it shows

- Generate C mocks (`engine_mock.c/.h`) and Racket binding (`mock/engine.rkt`)
- Load a shared library with `load-lib`
- Replace a C dependency with `(mock ...)`
- Invoke C from Racket with `(call ...)` and `(cfunc ...)`

## Run

```sh
make test
```

If `mockoto` is not in `PATH`:

```sh
make test MOCKOTO=/path/to/mockoto
```
