(in-ns 'wajure.core)

(def defmacro (macro ([sym & args]
                      (if (list? (first args))
                        `(def ~sym (macro ~@args))
                        `(def ~sym (macro ~args))))))

(defmacro fn [& args]
  (if (list? (first args))
    `(fn* ~@args)
    `(fn* ~args)))

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

;; (defn range [x] (print "TODO: implement range" x))
