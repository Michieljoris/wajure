(in-ns 'foo.core)

;; (clojure.core/use 'clojure.core)
(defn f [x y z] (print [x y z]))
(def fp (partial f 1))

(defn f2 [x] (print x))
(def x 123)
