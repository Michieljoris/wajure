(in-ns 'foo.core2)
(require '[bar.core :as bar:refer [bar]])

(def foo2 :foo2)
;; (def bar :bar)
(print "from foo.core2:", foo2 bar)
(defn f [x] (print "f fn in foo.core2" bar))
