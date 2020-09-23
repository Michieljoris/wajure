(in-ns 'test.test2-eval)

(defn f0 [] :f0)
(defn f1 [x] [x])
(defn f2 [x y] [x y])
(defn f2r [x y & z] [x y z])
;; (def f1a (partial f1 1))
;; (def f2a (partial f2 1))
;; (def f2ra (partial f2r 1 2))
;; (def f2rb (partial f2r 1 2 3))
;; (def f2rc (partial f2r 1 2 3 4))

;; (def plus2 (partial + 2))

;; (defn test-apply []

;;   (t (apply f0 nil) :f0)
;;   (t (apply f1 1 nil) [1])
;;   (t (apply f1 (list 1)) [1])
;;   (t (apply f2 1 2 nil) [1 2])
;;   (t (apply f2 1 (list 2)) [1 2])

;;   )

;; (defn test-partial []

;;   (t (f1a) [1])
;;   (t (f2a 2) [1 2])
;;   (t (f2ra) [1 2 nil])
;;   (t (f2rb) [1 2 (3)])
;;   (t (f2rc) [1 2 (3 4)])
;;   (t (f2rc) [1 2 (3 4)])
;;   (t (f2ra 5 6) [1 2 (5 6)])
;;   (t (f2rb 5 6) [1 2 (3 5 6)])
;;   (t (f2rc 5 6) [1 2 (3 4 5 6)])
;;   (t (f2ra) [1 2 nil])
;;   (t (f2rb) [1 2 (3)])
;;   (t (f2rc) [1 2 (3 4)])
;;   (t (f2rc) [1 2 (3 4)])
;;   (t (plus2 2) 4)

;;   )

;; Testing evaluation of arithmetic operations
(defn test []
  (pr "test2")
  (t (+ 5 (* 2 3))
     11)

  (t (- (+ 5 (* 2 3)) 3)
     8)

  (t (/ (- (+ 5 (* 2 3)) 3) 4)
     2)

  (t (/ (- (+ 515 (* 87 311)) 302) 27)
     1010)

  (t (* -3 6)
     -18)

  (t (/ (- (+ 515 (* -87 311)) 296) 27)
     -994)

;;; This should throw an error with no return value
  ;; TODO, it does but should be caught by test fn..
  ;; (t (abc 1 2 3) 2)

  ;; Testing empty list
  (t () ())

  ;; ;; Testing evaluation within collection literals
  (t [1 2 (+ 1 2)]
     [1 2 3])
 

  ;; ;; Check that evaluation hasn't broken empty collections
  (t []
     [])

  ;; TODO: Maps
  ;; (t {"a" (+ 7 8)}
  ;;    {"a" 15})

  ;; (t {:a (+ 7 8)}
  ;;    {:a 15})

  ;; (t {}
  ;;    {})

  ;; Test the test
  ;; (t 2 1)
  )
