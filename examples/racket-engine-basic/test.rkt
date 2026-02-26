#lang racket

(require rackunit
         "mock/engine.rkt")

(define args (current-command-line-arguments))
(when (< (vector-length args) 1)
  (error 'test.rkt "usage: racket test.rkt <shared-library-path>"))

(define lib-path (vector-ref args 0))
(load-lib lib-path)

(mock sequencer_capture
      (lambda (id)
        (+ id 100)))

(check-equal? (call engine_resume 3) 104)
(check-equal? (call engine_compute 3) 217)

(define raw-engine-compute (cfunc engine_compute))
(check-equal? (raw-engine-compute 1) 213)

(displayln "racket-engine-basic: ok")
