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
      "./libengine_chibi.dylib"))

(load-lib lib-path)

(returns sequencer_capture 100)

(assert-eq "engine-resume" 101 (call engine_resume 3))
(assert-eq "sequencer-called-after-resume" 1 (called sequencer_capture))

(assert-eq "engine-compute" 201 (call engine_compute 3))
(assert-eq "sequencer-called-after-compute" 2 (called sequencer_capture))

(define raw-engine-compute (cfunc engine_compute))
(assert-eq "engine-compute-raw" 201 (raw-engine-compute 1))
(assert-eq "sequencer-called-after-raw" 2 (called sequencer_capture))

(display "racket-engine-basic chibi: ok\n")
