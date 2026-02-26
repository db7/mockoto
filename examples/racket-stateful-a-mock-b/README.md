# racket-stateful-a-mock-b

Stateful `A` component tested from Racket while mocking stateful behavior of `B`.

## Pattern

- `A` keeps state in `a_state_t` (`counter`, `last`)
- `A` calls dependency `B` (`b_transform`) on each step
- Racket injects a stateful mock for `B` with `(mock b_transform ...)`
- Racket drives `A` with `(call a_init/a_step/a_total ...)`

This mirrors the lotto-style pattern: exercise real component logic while replacing
one dependency with a programmable mock at runtime.

## Run

```sh
make test
```

If `mockoto` is not in `PATH`:

```sh
make test MOCKOTO=/path/to/mockoto
```
