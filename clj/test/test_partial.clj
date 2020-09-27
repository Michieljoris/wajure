(in-ns 'test.test-partial)


(defn f [x y z] [x y z])
(defn g [x y & z] [x y z])
(def fp (partial f 1))
(def fp2 (partial fp 2))
(def gp (partial g 1 2))
;; (def plus (partial + 1))

;; (defn test-partial []
;;   ;; (print (fp 2 3))

;;   (let [f2 fp]
;;     (print"hello"  (f2 2 3)))

;;   )

(defn test-partial []

  (let [f (fn [x y z] [x y z])
        g (fn [x] x)]
    (t (f 2 (g 1) 3) [2 1 3] "pass results of fn to fns"))

  ;; Partials of global fns
  (let [f1 (partial f 1 2)
        g1 (partial g 1 2)]
    ;;No rest arg
    (t (f1 3) [1 2 3])
    ;;With rest arg
    (t (g1) [1 2 nil])
    (t (g1 3) [1 2 (3)])
    (t (g1 3 4) [1 2 (3 4)])
    (t (g1 3 4 5) [1 2 (3 4 5)])
    )
  ;; Partial global fns
  (t (fp 2 3) [1 2 3])
  (t (gp) [1 2 nil])
  (t (gp 3) [1 2 (3)])
  (t (gp 3 4) [1 2 (3 4)])
  (t (gp 3 4 5) [1 2 (3 4 5)])
  (t (gp 3 4 5 6) [1 2 (3 4 5 6)])

  ;; Partial of a global fn with no partials is just the global fn
  (let [fp (partial f)]
    (t (fp 1 2 3) [1 2 3])
    ;; Partials of non fn is identity fn
    (t (partial 1) 1)
    (t (partial "abc") "abc")
    )

  ;; Datafying of a global partial fn should still work
  (let [f2 fp]
    (t (f2 2 3) [1 2 3]))
  (let [f3 fp2]
    (t (f3 3) [1 2 3]))

  ;; Partial of a partial of a global fn
  (t (fp2 3) [1 2 3])

  ;; Making a partial of global partial fn
 (let [fp2 (partial fp 2)]
   (t (fp2 3) [1 2 3]))

  (pr "Partial tests have run")
  )
