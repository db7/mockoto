(import (chicken base))

(include "../../binders.chicken.scm")
(include "mock/a.chicken.scm")
(include "mock/b.chicken.scm")

(load-lib #f)

(define (assert-eq label expected got)
  (unless (equal? expected got)
    (print "FAIL: " label " expected=" expected " got=" got)
    (error label)))

(define (make-a-state-default)
  (handle-exceptions ex
    (make-a-state)
    (make-a-state 0 0)))

(define st (make-a-state-default))

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

(print "racket-stateful-a-mock-b chicken: ok")
