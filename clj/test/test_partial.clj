(in-ns 'test.test-partial)

(defn f [x y z] [x y z])
(defn g [x y & z] [x y z])
(def fp (partial f 1 2))
(def gp (partial g 1 2))


;; (defn test-partial []
;;   (let [f1 (partial f 0)]
;;     ;; (print f f1)
;;     (print (f1  1 2 3))
;;     )
;;   )

(defn test-partial []
  (let [f (fn [x y z] [x y z])
        g (fn [x] x)]
    (t (f 2 (g 1) 3) [2 1 3]))

  (let [f1 (partial f 1 2)
        g1 (partial g 1 2)]
    (t (f1 3) [1 2 3])

    (t (g1) [1 2 nil])
    (t (g1 3) [1 2 (3)])
    (t (g1 3 4) [1 2 (3 4)])
    (t (g1 3 4 5) [1 2 (3 4 5)])
    )
  (t (fp 3) [1 2 3])
  (t (gp) [1 2 nil])
  (t (gp 3) [1 2 (3)])
  (t (gp 3 4) [1 2 (3 4)])
  (t (gp 3 4 5) [1 2 (3 4 5)])
  (t (gp 3 4 5 6) [1 2 (3 4 5 6)])

  (let [fp (partial f)]
    (t (fp 1 2 3) [1 2 3])
    (t (partial 1) 1)
    (t (partial "abc") "abc")
    )
  )
