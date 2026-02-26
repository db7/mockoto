# pure-c-runtime-hooks

Pure C example resembling lotto unit tests that use generated C mocks directly.

## What it shows

- Generate `runtime_mock.c/.h` with `mockoto --mode c/h`
- Install hooks from C with `mockoto_memmgr_runtime_alloc_hook`
- Validate behavior and call counters from C

## Run

```sh
make test
```

If `mockoto` is not in `PATH`:

```sh
make test MOCKOTO=/path/to/mockoto
```
