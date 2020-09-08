(in-ns 'foo.core)

(require '[bar.core :as foo :refer [bar]])

(def foo :foo)
(def bar :bar)
(print "from foo.core:", foo bar)
(defn f [x] (print "f fn in foo"))
