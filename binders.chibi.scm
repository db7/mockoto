;; Runtime helpers for mockoto-generated Chibi bindings.
;;
;; Contract:
;;   load-lib, cfunc, call, mock, returns, called

(import (scheme base)
        (scheme eval)
        (scheme load))

(define mockoto-lib #f)
(define mockoto-mock-funs '())

(define (symbol->string-safe s)
  (if (symbol? s) (symbol->string s) s))

(define (combine-sym . parts)
  (string->symbol (apply string-append (map symbol->string-safe parts))))

(define (lookup-proc sym)
  (guard (ex (else (error "unknown function symbol" sym)))
    (let ((v (eval sym)))
      (if (procedure? v)
          v
          (error "not a callable symbol" sym)))))

(define (load-lib path)
  (set! mockoto-lib path)
  (if path
      (load path)
      #t))

(define (cfunc* sym)
  (lookup-proc sym))

(define (call* sym . args)
  (apply (cfunc* sym) args))

(define (mock* sym proc)
  (set! mockoto-mock-funs (cons proc mockoto-mock-funs))
  ((cfunc* (combine-sym "mockoto_" sym "_hook")) proc))

(define (returns* sym value)
  ((cfunc* (combine-sym "mockoto_" sym "_returns")) value))

(define (called* sym)
  ((cfunc* (combine-sym "mockoto_" sym "_called"))))

(define-syntax cfunc
  (syntax-rules ()
    ((_ name)
     (cfunc* 'name))))

(define-syntax call
  (syntax-rules ()
    ((_ name args ...)
     (call* 'name args ...))))

(define-syntax mock
  (syntax-rules ()
    ((_ name proc)
     (mock* 'name proc))))

(define-syntax returns
  (syntax-rules ()
    ((_ name value)
     (returns* 'name value))))

(define-syntax called
  (syntax-rules ()
    ((_ name)
     (called* 'name))))
