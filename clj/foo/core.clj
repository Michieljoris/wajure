(in-ns 'foo.core)

(defn f [x y z] [x y z])
(def fp (partial f 1))

;; (defn f []
;;   (let [plus (partial + 1 1)]
;;    (t (plus 1) 3)
;;    (print (plus 1))
;;    )

;;    )





;; (defn f [x y z] (print "in f") [x y z])
;; (def f2 f)
;; (def foo (f 1 2 3))
;; (defn f [x] (print "in redefined f") 1)
;; (clojure.core/use 'clojure.core)
;; (defn f [x y z] (print [x y z]))
;; (def fp (partial f 1))

;; (defn f2 [x] (print x))
;; (def x 123)
