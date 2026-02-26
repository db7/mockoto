# Mockoto Cross-Runtime Test API Contract (v1)

## Goal

Define one practical interface for test authors that is stable across:
- Racket
- Chibi Scheme
- CHICKEN Scheme

Runtime-specific FFI differences should be hidden in `binders.*` files.
Tests should prefer this contract instead of raw generated bindings.

## Scope

This contract defines:
- API names and call shapes used in tests
- return/argument normalization rules
- expected runtime errors

It does not require identical internal implementation in each runtime.

## Canonical Surface

All `binders.*` implementations should provide these operations:

1. `load-lib path`
   - Load the target dynamic library and set it as current binding context.

2. `call name arg ...`
   - Invoke C function `name`.
   - `name` is a symbol (for example `'tidset_insert`).

3. `mock name proc`
   - Install mock callback for function `name`.
   - `proc` must match the function arity and ABI-compatible argument/result mapping.

4. `returns name value`
   - Set default return value for generated mock function `name` using `mockoto_<name>_returns`.
   - Only valid for non-`void` functions.

5. `called name`
   - Return call count using `mockoto_<name>_called`.
   - Counter-reset behavior must match generated C mock semantics.

6. `cfunc name`
   - Return low-level callable for advanced use.
   - No additional normalization is required beyond runtime FFI conversion.

Implementations may provide sugar forms (identifier-based macros, aliases), but the symbol-based API above must exist and be documented.

## Type Mapping Rules

`binders.*` must normalize these behaviors consistently:

1. Integer C types (`int`, `size_t`, fixed-width ints, enums) map to exact integers.
2. C `_Bool` maps to Scheme booleans.
3. `char *` in string position maps to Scheme string.
4. Byte buffers (`uint8_t *`, `unsigned char *`, `int8_t *`) map to opaque pointer/byte-buffer handle, not Scheme string.
5. Struct/union pointers map to opaque pointers unless explicit accessor wrappers are provided.
6. Opaque/incomplete record pointers must remain opaque and never require record layout knowledge in tests.

## Error Contract

`binders.*` should raise clear runtime errors for:

1. Calling API before `load-lib`.
2. Referencing unknown function symbol.
3. Arity mismatch in `call`/`mock`.
4. Using `returns` on a `void` function.

Error wording may vary by runtime, but failure class must be unambiguous.

## Naming Contract

1. Canonical API function names are exactly:
   - `load-lib`, `call`, `mock`, `returns`, `called`, `cfunc`
2. Generated C symbol names remain unchanged.
3. Any extra aliases (`snake_case`, `kebab-case`, identifier macros) are optional sugar.

## Conformance

A runtime conforms to v1 when:

1. It exports the canonical surface.
2. It satisfies the type mapping and error rules above.
3. The same semantic test script can run with runtime-specific `binders.*` substitution only.
