#lang racket

(require rackunit
         (only-in "mock/a.rkt"
                  load-lib
                  call
                  alloc-a_state
                  _a_init
                  _a_step
                  _a_total)
         (only-in "mock/b.rkt"
                  [load-lib b-load-lib]
                  [mock b-mock]
                  [cfunc b-cfunc]
                  _b_transform))

(define args (current-command-line-arguments))
(when (< (vector-length args) 1)
  (error 'test.rkt "usage: racket test.rkt <shared-library-path>"))

(define lib-path (vector-ref args 0))
(load-lib lib-path)
(b-load-lib lib-path)

(define b-call-count 0)
(define bias 10)

(b-mock b_transform
        (lambda (acc x)
          (set! b-call-count (+ b-call-count 1))
          (+ (* acc 2) x bias)))

(define st (alloc-a_state))
(call a_init st 3)

(check-equal? (call a_step st 5) 21)
(check-equal? (call a_total st 0) 21)
(check-equal? (call a_step st 1) 19)
(check-equal? (call a_total st 4) 23)
(check-equal? b-call-count 2)

(define raw-b-transform (b-cfunc b_transform))
(check-equal? (raw-b-transform 7 2) 26)
(check-equal? b-call-count 3)

(displayln "racket-stateful-a-mock-b: ok")
