(import (chicken base))

(include "../../binders.chicken.scm")
(include "mock/engine.chicken.scm")
(include "mock/trace.chicken.scm")

(load-lib #f)

(define (assert-eq label expected got)
  (unless (equal? expected got)
    (print "FAIL: " label " expected=" expected " got=" got)
    (error label)))

(returns trace_load 7)

(assert-eq "engine-step" 8 (call engine_step 7))
(assert-eq "trace-save-called" 1 (called trace_save))
(assert-eq "trace-load-called" 1 (called trace_load))

(define raw-trace-load (cfunc trace_load))
(assert-eq "trace-load-raw" 7 (raw-trace-load 999))
(assert-eq "trace-load-called-raw" 1 (called trace_load))

(print "racket-dual-bindings chicken: ok")
