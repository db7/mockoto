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

(define (insert-many s vals)
  (for-each (lambda (v) (call tidset_insert s v)) vals))

(define vals '(1 2 3 4 5 10 34))
(define odds '(1 3 5))

(load-lib (runtime-lib-path))

(define s (call tidset_new))
(assert-eq "size-initial" 0 (call tidset_size s))
(insert-many s vals)
(assert-eq "has-3" #t (call tidset_has s 3))
(assert-eq "has-100" #f (call tidset_has s 100))
(assert-eq "size-after-insert" (length vals) (call tidset_size s))

;; sort wrappers avoid passing function pointers from Scheme
(call tidset_sort_gt_apply s)
(assert-eq "sort-gt-head" 1 (call tidset_get s 0))
(call tidset_sort_lt_apply s)
(assert-eq "sort-lt-head" 34 (call tidset_get s 0))

(define s2 (call tidset_new))
(insert-many s2 odds)
(call tidset_intersect s s2)
(assert-eq "contains-all-odds" #t (call tidset_contains_all s s2))

(insert-many s vals)
(call tidset_subtract s s2)
(assert-eq "subtract-no-1" #f (call tidset_has s 1))
(assert-eq "subtract-no-3" #f (call tidset_has s 3))
(assert-eq "subtract-no-5" #f (call tidset_has s 5))

(call tidset_filter_not_multiple_of_4 s)
(assert-eq "filter-no-4" #f (call tidset_has s 4))

(call tidset_delete s2)
(call tidset_delete s)

(display runtime-tag)
(display ": ok\n")
