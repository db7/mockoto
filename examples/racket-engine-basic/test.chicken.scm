(import (chicken base))

(include "../../binders.chicken.scm")
(include "mock/engine.chicken.scm")

(load-lib #f)

(define (assert-eq label expected got)
  (unless (equal? expected got)
    (print "FAIL: " label " expected=" expected " got=" got)
    (error label)))

(returns sequencer_capture 100)

(assert-eq "engine-resume" 101 (call engine_resume 3))
(assert-eq "sequencer-called-after-resume" 1 (called sequencer_capture))

(assert-eq "engine-compute" 201 (call engine_compute 3))
(assert-eq "sequencer-called-after-compute" 2 (called sequencer_capture))

(define raw-engine-compute (cfunc engine_compute))
(assert-eq "engine-compute-raw" 201 (raw-engine-compute 1))
(assert-eq "sequencer-called-after-raw" 2 (called sequencer_capture))

(print "racket-engine-basic chicken: ok")
