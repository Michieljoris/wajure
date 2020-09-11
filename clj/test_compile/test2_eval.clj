(in-ns 'test-compile.test2-eval)

;; Testing evaluation of arithmetic operations
(defn test []
  (if false 1)
  (if false 1)
  ;; (let [evalled 1]
  ;;   (when (not= evalled (quote 1))
  ;;     (do
  ;;       (pr "FAIL" "foo")
  ;;       (pr (quote 1) )
  ;;       (pr "Expected:" (quote 1))
  ;;       (pr "Got:" 1))))
  ;; (t 1 1)
  ;; (let [evalled 1]
  ;;   (when (not= evalled (quote 1))
  ;;     (do
  ;;       (pr "FAIL" "foo")
  ;;       (pr (quote 1) )
  ;;       (pr "Expected:" (quote 1))
  ;;       (pr "Got:" 1))))
  ;; (t 1 1)
  ;; "foo"


  ;; (t (+ 5 (* 2 3))
  ;;    11)

  ;; (t (- (+ 5 (* 2 3)) 3)
  ;;    8)

  ;; (t (/ (- (+ 5 (* 2 3)) 3) 4)
  ;;    2)

  ;; (t (/ (- (+ 515 (* 87 311)) 302) 27)
  ;;    1010)

  ;; (t (* -3 6)
  ;;    -18)

  ;; (t (/ (- (+ 515 (* -87 311)) 296) 27)
  ;;    -994)

  ;; (t 1 1)
;;; This should throw an error with no return value
  ;; TODO, it does but should be caught by test fn..
  ;; (t (abc 1 2 3) 2)

  ;; Testing empty list
  ;; (t () ())

  ;; ;; Testing evaluation within collection literals
  ;; (t [1 2 (+ 1 2)]
  ;;    [1 2 3])

  ;; ;; Check that evaluation hasn't broken empty collections
  ;; (t []
  ;;    [])

  ;; TODO: Maps
  ;; (t {"a" (+ 7 8)}
  ;;    {"a" 15})

  ;; (t {:a (+ 7 8)}
  ;;    {:a 15})

  ;; (t {}
  ;;    {})
  )
