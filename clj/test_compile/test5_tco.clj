(in-ns 'test.test5-tco)
;; Testing recursive tail-call function

;; (def sum2 (fn [n acc] (if (= n 0) acc (sum2 (- n 1) (+ n acc)))))

;; TODO: test let*, and do for TCO

;; (t (sum2 10 0) 55)

;;TODO TEST, implement loop recur
;; (def res2 nil)
;=>nil
;; (def res2 (sum2 10000 0))
;; res2
;=>50005000


;; Test mutually recursive tail-call functions

;; (def foo (fn [n] (if (= n 0) 0 (bar (- n 1)))))
;; (def bar (fn [n] (if (= n 0) 0 (foo (- n 1)))))

;; (t (foo 10) 0)
;=>0
