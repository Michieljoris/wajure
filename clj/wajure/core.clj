(in-ns 'wajure.core)

(def defmacro (macro ([sym & args]
                      (if (list? (first args))
                        `(def ~sym (macro ~@args))
                        `(def ~sym (macro ~args))))))

;; (defmacro fn [& args]
;;   (if (list? (first args))
;;     `(fn* ~@args)
;;     `(fn* ~args)))

(defmacro fn [& args]
  (let [symbol (first args)]
    (if (symbol? symbol)
      (let [args (rest args)]
        (if (list? (first args)) `(fn* ~symbol ~@args)
            `(fn* ~symbol ~args)))
      (if (list? (first args))
        `(fn* ~@args)
        `(fn* ~args)))))

(defmacro defn [sym & args]
  `(def ~sym (fn ~@args)))

(defmacro cond [& xs]
  (if (> (count xs) 0)
    (list 'if (first xs)
          (if (> (count xs) 1)
            (nth xs 1)
            (throw "odd number of forms to cond"))
          (cons 'cond (rest (rest xs))))))

(defmacro when [cond body]
  `(if ~cond ~body))

(defmacro not [b]
  `(if ~b false true))

(defmacro t [expr expected & str]
  `(let [evalled ~expr]
     (when (not= evalled (quote ~expected))
      (do
        (pr "FAIL" ~@str)
        (pr (quote ~expr) )
        (pr "Expected:" (quote ~expected))
        (pr "Got:" ~expr))
      )))

(defmacro macroexpand-all [form]
  (list 'let ['form `(macroexpand ~form)]
        '(if (list? form)
           (if (> (count form) 0)
             (cons (macroexpand-all (first form)) (macroexpand-all (rest form)))
             form)
           form)))

;; TODO:
;; Full clojure.core macro: (needs map and meta)
;; (defmacro declare
;;   "defs the supplied var names with no bindings, useful for making forward declarations."
;;   {:added "1.0"}
;;   [& names] `(do ~@(map #(list 'def (vary-meta % assoc :declared true)) names)))

;;TODO: we need map for this macro, and reader fns.
;; (defmacro declare
;;   "defs the supplied var names with no bindings, useful for making forward declarations."
;;   {:added "1.0"}
;;   [& names] `(do ~@(map #(list 'def %) names)))

;;TODO: needs map
;; (defmacro declare
;;   "defs the supplied var names with no bindings, useful for making forward declarations."
;;   {:added "1.0"}
;;   [& names] `(do ~@(map (fn [name] (list 'def name)) names)))

(defmacro declare [name]
  `(def ~name))

;; (defn foo {:a :b} [x] x)
;; (meta #'foo)
;; (defn range [x] (print "TODO: implement range" x))
