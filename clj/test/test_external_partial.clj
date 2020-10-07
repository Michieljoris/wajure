(in-ns 'test.test-external-partial)

(defn f [x y z] [x y z])
(def fp (partial f 1))

(defn f [x] 1)
