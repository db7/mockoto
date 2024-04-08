#lang racket

(module binders racket
  (provide load-lib
           cfunc
           call
           mock)

  (require ffi/unsafe)
  (require (for-syntax racket/syntax))

  ;; ---------------------------------------------------------------------------
  ;;
  ;; ---------------------------------------------------------------------------

  ;; add the _ prefix to symbols
  (define-syntax (add-prefix stx)
    (syntax-case stx ()
      [(add-prefix pfx name)
       (format-id #'name "~a~a" #'pfx (syntax-e #'name))]))

  ;; get ffi obj for _sym and define it as 'sym
  (define-syntax-rule (define-ffi clib sym)
    (define sym (get-ffi-obj 'sym clib (add-prefix "_" sym))))

  (define (combine-sym . args)
    (let ([strs (map (lambda (s) (if (symbol? s) (symbol->string s) s)) args)])
      (string->symbol (apply string-append strs))))

  (define-syntax-rule (mockoto-hook clib fun-sym mock-fun)
    (let* ([fun-type (add-prefix "_" fun-sym)]
           [hook-type (_fun fun-type -> _void)]
           [hook-sym (combine-sym "mockoto_" 'fun-sym "_hook")]
           [hook-fun (get-ffi-obj hook-sym clib hook-type)])
      (hook-fun mock-fun)))

  (define-syntax-rule (get-ffi clib fun-sym)
    (let* ([fun-type (add-prefix "_" fun-sym)] ;
           [real-fun (get-ffi-obj 'fun-sym clib fun-type)])
      real-fun))

  (define-syntax-rule (call-ffi clib fun-sym args ...) ;
    ((get-ffi clib fun-sym) args ...))

  ;; the user can define here one library
  (define mockoto-lib '())

  (define (load-lib path #:custodian [custodian #f])
    (set! mockoto-lib (ffi-lib path #:custodian custodian)))

  (define-syntax-rule (mock fun-sym mock-fun)
    ;; ensure mockotolib set
    (mockoto-hook mockoto-lib fun-sym mock-fun))

  (define-syntax-rule (call fun-sym args ...)
    ;; ensure mockotolib set
    (call-ffi mockoto-lib fun-sym args ...))

  (define-syntax-rule (cfunc sym)
    ;; ensure mockotolib set
    (get-ffi mockoto-lib sym)))

;; -----------------------------------------------------------------------------
;; binders public interface
;; -----------------------------------------------------------------------------
(require 'binders)
(provide (all-from-out 'binders)
         (all-defined-out))
