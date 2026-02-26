(import (scheme base)
        (scheme load)
        (scheme process-context)
        (scheme write))

(load "../../binders.chibi.scm")

(define (assert-eq label expected got)
  (unless (equal? expected got)
    (display "FAIL: ")
    (display label)
    (display " expected=")
    (write expected)
    (display " got=")
    (write got)
    (newline)
    (error label)))

(define args (command-line))
(define lib-path
  (if (> (length args) 1)
      (cadr args)
      "./libpipeline_chibi.dylib"))

(load-lib lib-path)

(define state -1)

(returns trace_load 7)

(assert-eq "engine-step" 8 (call engine_step 7))
(assert-eq "trace-save-called" 1 (called trace_save))
(assert-eq "trace-load-called" 1 (called trace_load))

(define raw-trace-load (cfunc trace_load))
(assert-eq "trace-load-raw" 7 (raw-trace-load 999))
(assert-eq "trace-load-called-raw" 1 (called trace_load))

(display "racket-dual-bindings chibi: ok\n")
