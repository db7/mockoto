(import (scheme base)
        (scheme load)
        (scheme process-context)
        (scheme write))

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
      "./libtidset_chibi.dylib"))

(load lib-path)

(define (insert-many s vals)
  (for-each (lambda (v) (tidset_insert s v)) vals))

(define vals '(1 2 3 4 5 10 34))
(define odds '(1 3 5))

(define s (tidset_new))
(assert-eq "size-initial" 0 (tidset_size s))
(insert-many s vals)
(assert-eq "has-3" #t (tidset_has s 3))
(assert-eq "has-100" #f (tidset_has s 100))
(assert-eq "size-after-insert" (length vals) (tidset_size s))

;; sort wrappers avoid passing function pointers from Scheme
(tidset_sort_gt_apply s)
(assert-eq "sort-gt-head" 1 (tidset_get s 0))
(tidset_sort_lt_apply s)
(assert-eq "sort-lt-head" 34 (tidset_get s 0))

(define s2 (tidset_new))
(insert-many s2 odds)
(tidset_intersect s s2)
(assert-eq "contains-all-odds" #t (tidset_contains_all s s2))

(insert-many s vals)
(tidset_subtract s s2)
(assert-eq "subtract-no-1" #f (tidset_has s 1))
(assert-eq "subtract-no-3" #f (tidset_has s 3))
(assert-eq "subtract-no-5" #f (tidset_has s 5))

(tidset_filter_not_multiple_of_4 s)
(assert-eq "filter-no-4" #f (tidset_has s 4))

(tidset_delete s2)
(tidset_delete s)

(display "tidset-chibi: ok\n")
