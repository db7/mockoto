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
      "./liba_chibi.dylib"))

(load-lib lib-path)

(define st (make-a-state))

(call a_init st 3)

(returns b_transform 21)
(assert-eq "a-step-5" 21 (call a_step st 5))
(assert-eq "b-called-after-step1" 1 (called b_transform))
(assert-eq "a-total-0" 21 (call a_total st 0))

(returns b_transform 19)
(assert-eq "a-step-1" 19 (call a_step st 1))
(assert-eq "b-called-after-step2" 1 (called b_transform))
(assert-eq "a-total-4" 23 (call a_total st 4))

(define raw-b-transform (cfunc b_transform))
(returns b_transform 26)
(assert-eq "raw-b-transform" 26 (raw-b-transform 7 2))
(assert-eq "b-called-after-raw" 1 (called b_transform))

(display "racket-stateful-a-mock-b chibi: ok\n")
