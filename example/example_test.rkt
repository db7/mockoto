#lang racket

(require rackunit
         "example.rkt")
(load-lib "libexample.so")

(check-equal? (call client_action 1) 1)
(check-equal? (call client_action 2) 100)
(check-equal? (call client_action 4) 123)

(mock server_action
      (lambda (msg)
        (printf "Message: ~a\n" msg)
        1000))

(check-equal? (call client_action 1) 1001)
(check-equal? (call client_action 2) 1100)
(check-equal? (call client_action 4) 123)
