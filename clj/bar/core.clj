(in-ns 'bar.core)
;; (require '[foo.core :as foo :refer [foo]])


;; (defn f [] (list foo.core/foo 1234))

;; (def bar (f))


;; (def fox :fox-in-bar)


;; (def bar fox)
;; (def bar :bar-in-bar)
;; (def bar-again bar)





;; (def bar (list fox  1234))
;; (def bar (list fox foo.core/foo 1234
;;                ))
(def bar (list 123))


;; (print "from bar.core:", bar)
;; (defn bar-fn [] 123)
