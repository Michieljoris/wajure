(in-ns 'foo.core)

;; (clojure.core/use 'clojure.core)
(defn f [x y z] [x y z])
(def fp (partial f 1))

