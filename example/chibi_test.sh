#!/bin/sh

set -eu

MOCKOTO="${MOCKOTO:-../build/mockoto}"
OUT="${OUT:-example.stub}"

"$MOCKOTO" --mode chibi client.h server.h -- -I. > "$OUT"

grep -F '(c-include "client.h")' "$OUT" >/dev/null
grep -F '(c-include "server.h")' "$OUT" >/dev/null
grep -F '(define-c int client_action (int))' "$OUT" >/dev/null
grep -F '(define-c int server_action (string))' "$OUT" >/dev/null

if command -v chibi-ffi >/dev/null 2>&1; then
  chibi-ffi "$OUT" >/dev/null
fi

echo "chibi stub test passed: $OUT"
