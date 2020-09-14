(in-ns 'bar.core)
(require '[foo.core :as foo :refer [foo]])

(defn f [] foo.core/foo)

(def bar (f))
;; (print "from bar.core:", bar)
(defn bar-fn [] 123)
