#lang racket

(require rackunit
         (only-in "mock/engine.rkt"
                  load-lib
                  call
                  _engine_step)
         (only-in "mock/trace.rkt"
                  [load-lib trace-load-lib]
                  [mock trace-mock]
                  [cfunc trace-cfunc]
                  _trace_save
                  _trace_load))

(define args (current-command-line-arguments))
(when (< (vector-length args) 1)
  (error 'test.rkt "usage: racket test.rkt <shared-library-path>"))

(define lib-path (vector-ref args 0))
(load-lib lib-path)
(trace-load-lib lib-path)

(define state -1)
(trace-mock trace_save (lambda (id) (set! state id)))
(trace-mock trace_load (lambda (id) state))

(check-equal? (call engine_step 7) 8)
(check-equal? state 7)

(define raw-trace-load (trace-cfunc trace_load))
(check-equal? (raw-trace-load 999) 7)

(displayln "racket-dual-bindings: ok")
