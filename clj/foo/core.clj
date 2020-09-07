(in-ns 'foo.core)

(require '[bar.core :as foo :refer [bar]])

(def foo :foo)
(print "from foo.core:", foo bar)
