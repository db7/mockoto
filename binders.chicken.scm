;; Runtime helpers for mockoto-generated CHICKEN bindings.
;;
;; Contract:
;;   load-lib, cfunc, call, mock, returns, called

(import (chicken base))
(import (chicken foreign))
(import (chicken eval))

(define mockoto-lib #f)
(define mockoto-mock-funs '())

(define (symbol->string-safe s)
  (if (symbol? s) (symbol->string s) s))

(define (combine-sym . parts)
  (string->symbol (apply string-append (map symbol->string-safe parts))))

(define (lookup-proc sym)
  (handle-exceptions ex
    (error "unknown function symbol" sym)
    (let ((v (eval sym)))
      (if (procedure? v)
          v
          (error "not a callable symbol" sym)))))

(define (load-lib . maybe-path)
  (set! mockoto-lib (if (null? maybe-path) #f (car maybe-path)))
  (cond
    ((or (null? maybe-path) (not (car maybe-path))) #t)
    (else (load-library (car maybe-path)))))

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
