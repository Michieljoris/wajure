(in-ns 'test-compile.test3-env)

(def z (+ 1 7))

(def mynum 111)
(def MYNUM 222)
(def x 4)
(def y (let [z 7] z))
(def a 4)



(defn test []
  (pr "test3")
  (t 1 1 "Just testing the testing")

 
  ;; Testing REPL_ENV
  (t (/ (- (+ 5 (* 2 3)) 3) 4)
     2)


  ;; Testing def!
  ;; (t (def x 3) nil)
  ;; (t x 3)
  (t z 8)
                                        ;=>8

  ;; Verifying symbols are case-sensitive
  (t mynum 111)
  (t MYNUM 222))


;;TODO TEST
;; Check env lookup non-fatal error
;; (abc 1 2 3)
;/.*\'?abc\'? not found.*
;; Check that error aborts def!
;; (def w 123)
;; (def w (abc))
;; (t w 123)
;=>123

;; Testing let*
(t (let [z 9] z) 9)
(t (let [x 9] x) 9)
(t x 4)
(t (let [z (+ 2 3)] (+ 1 z)) 6)
(t (let [p (+ 2 3) q (+ 2 p)] (+ p q)) 12)
(t y 7)

;; ;; Testing outer environment
(t (let [q 9] q) 9)
(t (let [q 9] a) 4)
(t (let [z 2] (let [q 9] a)) 4)

;; ;; Testing let* with vector bindings
(t (let [z 9] z) 9)
(t (let [p (+ 2 3) q (+ 2 p)] (+ p q)) 12)

;; ;; Testing vector evaluation
(t (let [a 5 b 6] [3 4 a [b 7] 8])
   [3 4 5 [6 7] 8])

