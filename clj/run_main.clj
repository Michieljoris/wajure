(in-ns 'run-main)
;; (require '[main :refer [main]])
;; (require '[foo.core :as foo])
;; (defmacro fn [& sigs]
;;   (print (first sigs))
;;   )
;; (defn f [x] x)
;; (print "----------")


;; (print (macroexpand '(fn [x] x) ))
;; (print (macroexpand '(fn ([x] x)
;;                           ([x y] [x y])) ))

;; (def defmacro (macro ([sym & args]
;;                       (if (list? (first args))
;;                         `(def ~sym (macro ~@args))
;;                         `(def ~sym (macro ~args))))))

;; (defmacro fn [& args]
;;   (if (vector? (first args))
;;     `(fn* ~args)
;;     `(fn* ~@args)))

(defmacro when2 [cond body]
  `(if ~cond ~body))
(print (macroexpand '(when 1 2)))
(print (macroexpand '(when2 1 2)))

;; (defn f [x] (+ x 1))
;; (print (apply f '(1)))
;; (defmacro foo [x] x)


;; (print (macroexpand '(defmacro foo ([x] x))))
;; (def fn (macro ([& args]
;;                 (if (vector? (first args))
;;                   `(fn* ~args)
;;                   `(fn* ~@args)))))

;; (def f (fn ([x] x) ([x y] [x y])) )
;; (print (f 1 2))
;; (def f (fn ([x] x) ([x y] [x y ])))
;; (def mymacro (macro ([x] `(+ 1 ~x))))
;; (defmacro mymacro [x] `(+ 1 ~x))
;; (print (macroexpand '(mymacro 2)))
;; (print (mymacro 1))
;; (def f (fn*
;;         ([] "foo")
;;         ([x y & z] [x y z])
;;         ([x y] [x y])
;;         ([x] [x])
;;         ))

;; (defn f ([] "foo")
;;         ([x y & z] [x y z])
;;         ([x y] [x y])
;;         ([x] [x])
;;         )
;; (def g (fn* ([x y] [x y])))
;; (print f)
;; (print (f))
;; (print (f 1))
;; (print (f 1 2))
;; (print (f 1 2 3))
;; (print (g 5 6))

;; (print foo/f)

;; (defmacro when [] 1)
;; (print when)
;; (foo 1)

;; (defn foo
;;   ([x] [x])
;;   ([x y] [x y]))



;; (main 1 2)
;; (require '[test.tests])

;; (defn g [x y] x y)
;; (defn f0 [] :f0)
;; (defn f1 [x] [x])
;; (defn f2 [x y] [x y])
;; (defn f2r [x y & z] [x y z])
;; (partial f 1 2 3 )
;; (print (f 2 3 4))
;; (print (f 2 3 4))

;; (def f1a (partial f1 1))
;; (def f2a (partial f2 1))
;; (def f2ra (partial f2r 1 2))
;; (def f2rb (partial f2r 1 2 3))
;; (def f2rc (partial f2r 1 2 3))

;; (t (f1a) [1])
;; (t (f2a 2) [1 2])
;; (t (f2ra) [1 2 nil])
;; (t (f2rb) [1 2 (3)])
;; (t (f2rc) [1 2 (3)])
;; (t (f2rc) [1 2 (3)])

;; (print (f2rc) )
;; (print (f2rc) )
;; (t (f2ra 5 6) [1 2 (5 6)])
;; (t (f2rb 5 6) [1 2 (3 5 6)])
;; (t (f2rc 5 6) [1 2 (3 4 5 6)])

;; (t (f1a) [1])
;; (t (f2a 2) [1 2])
;; (t (f2ra) [1 2 nil])
;; (t (f2rb) [1 2 (3)])
;; (t (f2rc) [1 2 (3 4)])
;; (t (f2rc) [1 2 (3 4)])
;; (t (f2ra 5 6) [1 2 (5 6)])
;; (t (f2rb 5 6) [1 2 (3 5 6)])
;; (t (f2rc 5 6) [1 2 (3 4 5 6)])
;; (print (macroexpand '(t 1 1)))
;; (defmacro m[] :foo)
;; (print (macroexpand '(m)))

;; (defmacro t2 [expr expected & str]
;;   `(let [evalled ~expr]
;;      (when (not= evalled (quote ~expected))
;;       (do
;;         (pr "FAIL" ~str)
;;         (pr (quote ~expr) )
;;         (pr "Expected:" (quote ~expected))
;;         (pr "Got:" ~expr))
;;       )))

;; (print (macroexpand '(t2 1 1)))

;; (t (apply f0 nil) :f1)

  ;; (t (apply f0 nil) :f0)
  ;; (t (apply f1 1 nil) [1])
  ;; (t (apply f1 (list 1)) [1])
  ;; (t (apply f2 1 2 nil) [1 2])
  ;; (t (apply f2 1 (list 2)) [1 2])
;; (defn g [x & y] [x y])
;; (def q (partial let))
;; (def f1 (partial f 1 ))
;; (def f2 (partial f1 2 ))
;; (def g (f 1))
;; (def plus (+ 1))
;; (print (f 1 2 3) (g 1 2 3))
;; (print ">>>>> " (f1 2  ) )
;; (print ">>>>> " (f2 3) )

;;TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!
;; (def plus2 (partial + 2 3))
;; (print (plus2 2))
;;TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!

;; (print (apply f0))
;; (print (apply f0 nil))
;; (print (apply f1 1 nil))
;; (print (apply f1 (list 1)))
;; (print (apply f2 1 2 nil))
;; (print (apply f2 1 (list 2)))
;; (pr "hello from run.clj!!")
;; (test2/test)

;; (require '[foo.core :as foo :refer [foo]])
;; (print "foo.core/foo = " foo.core/foo)
;; (print "foo/foo = " foo/foo)
;; (print "foo" foo)


;; (print "foo2" foo2)
;; (print "foo3" foo3)
;; (def foo 123)
;; (defn foo [] 123)
;; (print (foo))
;; (print 123)

;; (print (and 1 nil 0 true))
;; (print (macroexpand (apply print (list "hello"))))
;; (apply print (list "hello" "foo"))

;; (load "lispy/test/test_2_eval.lispy")
;; (load "lispy/test/test_3_env.lispy")
;; (load "lispy/test/test_4_if_fn_do.lispy")
;; (load "lispy/test/test_5_tco.lispy")
;; (load "lispy/test/test_6_file.lispy")
;; (load "lispy/test/test_7_quote.lispy")
;; (load "lispy/test/test_8_macros.lispy")
;; (load "lispy/test/test_9_try.lispy")
;; (print "----------------------")
;; (print "Namespace:" *ns*)

;; (require '[foo.core :as foo :refer [a b]])
;; (print-env)
;; (def bar 555)
;; (print foo bar)


;; (defmacro unless2 [pred a b] (list 'if (list 'not pred) a b))
;; (print (defmacro unless2 [pred a b] `(list )))

;; (t (macroexpand '(unless2 2 3 4))
;;    (if (not 2) 3 4))
;; (print (macroexpand '(unless2 2 3 4)))
;; (print (macroexpand  '(t2 (+ 1 2) 3 "Should be equal")))
;; (t (+ 1 2) 4 "Testing addition")

;; (def foo 123)
;; (defn test [a a1]
;;   (let [z 123
;;         f (fn [b] (let [c 99 d 99]  a a1 b c z
;;                     ))]
;;     1))
;; (def a 1)
;; (defn f [x]
;;   (let [q (fn [y] (print "HELLO" x y) )]
;;     q)
;;   )

;; (defn test [x y]
;;   (print  123 x y)
;;   ;; (let [ g (fn [x] (print "G" x))
;;   ;;        f (fn [x y] (g (+ x y)))
;;   ;;       ;; some-fn (f 1)
;;   ;;       ]

;;   ;;   (f 2 3)
;;   ;;   ;; (some-fn 2)
;;   ;;   ;; (g 123 4546)
;;   ;;   )
;;   )
;; (test 222 333)
;; (defn test [b]
;;   (let [c 2]
;;     (fn [d]
;;       (let [e 3]
;;         (fn [f]
;;           (+ a b c d e f))))))

;; (defn test [b]
;;     (fn [b] (+ a b))
;;   ;; (let [a 1 b 2 c 3 d 4 e 5]
;;   ;;      (fn [b] (+ a b)))
;;   )


;; (defn test [a b]
;;   (let [c 1] a b c 123))
;; (def bar `(1 ~foo  a 3))
;; (def bar '(1 2 3))
;; (defn my-fn [x] (+ x 1))

;; (defn test[x] 456 nil true false "ok")
;; (defn test[x] bar foo "ok" "then" 123 false true nil)
;; (defn test[x] "aaaaaa")
;; (defn test[x] (print (cons "hello" (list 1 (+ 2 3) (list 3 5)))))
;; (defn test[x] (print (nth (list 0 "one" 2) 1)))
;; (defn test2[x] (my-fn 1))

;; (defn test3a[x]
;;   (let [fn2 my-fn]
;;     (fn2 1)))

;; (defn test3b[x]
;;   (let [fn2 (partial my-fn 123)]
;;     (fn2 1)))

;; (defn test4[x]
;;   (let [a 1
;;         some-fn (fn [x] (+ a 1))
;;         fn2  some-fn
;;         fn3 (partial some-fn 123)]
;;     (some-fn 1)))
;; (defn test[x] ())


;; (defn test[x] (+ 1 2))


;; (defn foo[x] "some string" "string 2")

;; (defn bar[x] (+ 1 2))
;; (defn foo [x] (+ 1 x))
;; (defn fox [x] (fn [] x))
;; (defn foz [x] (let [a (+ x 1)
;;                     a (+ a 1)
;;                     b 123]
;;                 (fn [x y] (+ a b x y bar))))

;; (fn [x y a b] (+ a b x y bar))

;; {:wasm-fn-ref 1
;;  :parameter-count 2
;;  :parameters-list (42)
;;  :closure-list (5 7)
;;  }



;; ((foz 1) 1)


;; (def foo 123)
;; (def fox "abc")
;; (print-env)
;; (let [a foo]
;;   (def fuz a))
;; (def fux (let [a 1] a))
;; (defn bar [x] (let [f (fn [] (+ foo x))]
;;                f))

;; (defmacro m0 [x y] `(+ ~x ~y))
;; (defmacro m1 [x y] `(+ ~x ~y (m0 6 7)))
;; (def m2 (macro [y] `(m1 (m1 2 3) ~y)))
;; ;; (defmacro m2 [y] `(+ 123 ~y))
;; (print foo)
;; ((bar 123))

;; (macroexpand-all '(m2 123))

;; (defn me [form]
;;   (macroexpand form)
;;   )

;; (me '(m2 123))
;; (print "foo")


;; (macroexpand-all '(m2 1))
;; (and (+ 1 2) (= 1 2) (print "FOOOOO"))
;; (macroexpand (list 1 2))

;; (and false (print "foo"))


;; (macroexpand-all '(a b c))
;; (and (= 1 0) true)

;; (macroexpand '(apply + '(1 2)))
;; (apply and '(1 2))
;; (cons (first '(a b c))  (rest '(a b c)))
;; (+ 1 2)
;; (slurp "lispy/stdlib.lispy")
;; (slurp "lispy/test/test_2_eval.lispy")
;; (slurp "lispy/test/test_3_env.lispy")
;; (slurp "lispy/test/test_4_if_fn_do.lispy")
;; (slurp "lispy/test/test_5_tco.lispy")
;; (slurp "lispy/test/test_6_file.lispy")
;; (slurp "lispy/test/test_7_quote.lispy")
;; (slurp "lispy/test/test_8_macros.lispy")
;; (slurp "lispy/test/test_9_try.lispy")

;; ;; (print "Hello!!" (+ 2 2))
;; (print "Hash" (hash "foo"))
;; (print "Hash" (hash "foo"))

;; (print (boolean 123))
;; (print (hash 2))
;; (list 1 2)
;; (print (hash (cons 1 (list 2 3))))
;; (print (hash (cons 1 (list 2 (+ 2 1)))))
;; (print (hash (cons 1 '(2 3))))
;; (print (hash (rest (cons 1 (list 2 3)))))
;; "ok"
;; foobar
;; (t ( (fn [& more] (count more)) 1 2 3)
;;    3)
;; (t ( (fn [& more] (list? more)) 1 2 3)
;;    true)
;; "ok"
;; foo?
;; foo?
;; (t ( (fn [& more] (count more)) 1)
;;    1)
;; (t ( (fn [& more] (count more)) )
;;    0)
;; ;; (t ( (fn [& more] (list? more)) )
;; ;;    true)
;; (t ( (fn [a & more] (count more)) 1 2 3)
;;    2)
;; (t ( (fn [a & more] (count more)) 1)
;;    0)
;; (t ( (fn [a & more] (list? more)) 1)
;;    true)
;; (t (not nil)
;;    true)
;; (t (not true)
;;    false)
;; (t (not "a")
;;    false)
;; (t (not 0)
;;    false)

;; (t (= "" "")
;;    true)
;; (nth [1 2] 123)
;; (cond)
;; (not (not 1))
;; (defmacro unless2 [pred a b] (list 'if (list 'not pred) a b))
;; (print "hello")
;; (macroexpand '(cond 1 2))
;; (cond 0 1 1 2 1 3kkk)
;; (nth [1 2 3] 1)
;; (cond 1 7)
;; (slurp "lispy/test/test_9_try.lispy")
;; (t 1 0)
;; (+ 1 1)
;; (+ 1 1)
;; (+ 1 a)
;; (+ 1 123)
;; foo
;; ((fn [a b] (+ b a)) 3 4)

;; ((fn [f x] (f x))
;;  (fn [a] (+ 1 a))
;;  7)
;; ( ( (fn [a] (fn [b] (+ a b))) 5) 7)

;; (def x 4)
;; (defmacro a [] x)
;; (a)
;; ;=>2
;; (let [x 3]
;;   (print x)
;;   (a))
;; (let [foo (fn [n] (if (= n 1) (foo 0)))] (print-env) (foo 1))
;; (let [cst (fn [n] (if (= n 0) 0 (cst (- n 1))))] (cst 1))

;; (def fib (fn [N] (if (= N 0) 1 (if (= N 1) 1 (+ (fib (- N 1)) (fib (- N 2)))))))
;; =>2
;; (if nil 7 8)
;; 8
;; (if (list) 7 8) 7

;; (t (a 1 2 3) 2)
;; (def expr '(abc 1 2 3))
;; (when (not= (eval expr) 2)
;;   (do (pr expr))
;;   )

;; (+ 1 (+ 1 1))
;; (+ (+ 5 (* 2 3)) 3)
;; (defn x [y] y)
;; (x 1)

;; (pr '(+ 1 2))
;; (t (+ 1 2) 3)
;; (print "hello")
;; (defmacro when [cond body]
;;   `(if ~cond ~body))

;; (def t (macro [] (do 1 2 3)))
;; (def t (macro [] (do 1)))

;; (macroexpand '(t))
;; (defmacro when [cond body]
;;   `(if ~cond ~body))

;; (macroexpand '(t ))
;; (macroexpand '(when 1 2))
;; (when 1 2)
;; (load "lispy/test/test_2_eval.lispy")

;; 1
;; (+ 1)
;; [ 1 ]
;; '(1 2 3)
;; (cons 0 '(1 2 3))
;; (first '(1 2 3))
;; (rest  '(1 2 3))
;; (list 1 2 3)
;; (concat '(1 2) '(3 4))
;; (concat '(1 2) '())
;; (concat '() '(3 4))
;; (def a "FOOOOOOOOOOOOOOOOOOo")
;; (def a "UUUUUUUUUUUUUUUUU")
;; (pr a)
;; (if 1 2 3)
;; (fn [a b c] a)

;; (def a "baaaaaaaaaaaaaaar")
;; (+ 1 1)
;; `(a ~@(list a))


;; (try
;;  123
;;   (throw "foo")
;;   (catch E e e)
;;   (finally (pr ":finally")))

;; (let [a "THIS IS A!!!!!!"
;;       b 1
;;       c [1 2 3]
;;       ]
;;  'a
;;   456
;;   (let [d c]
;;    'b 'c
;;     (let [e d]
;;      'd
;;       b))
;;  123
;;   )

;; (let [] 123 'd 3)


;; (def f (fn [a] a))
;; (f 123)

;; (def defmacro (macro [sym params & body]
;;                      `(def ~sym (macro ~params ~@body))))

;; (macroexpand '(defmacro ))

;; (defmacro defn [sym params & body])

  ;; `(def ~sym (fn ~params ~@body)))

  ;; `(def ~sym (fn ~params ~@body))

;; (def defn (macro [sym params & body] `(def ~sym (fn ~params ~@body))))

;; (defn x [y] y)
;; (macroexpand '(defmacro x [y] y))
;; (macroexpand '(defn x [y] y))
;; (x 123)
;; (print-env)
;; (defn x [x y z]
;;   (let [ret (+ x y z)]
;;     ret))

;; (def foo (x 1 2 ))
;; 3

;; (def foo (macro [sym params & body] `(def ~sym (macro ~params ~@body))))
;; (def foo (fn [& args] args))
;; (foo 1 2 3)
;; (foo x [y] 'b1 'b2)
;; (print "&&&&&&&&&&&&&&&&" (macroexpand '(foo x [x y]  1 2)))
;; (foo x [x y]  1 2)
;; (print-env)
;; (def x (macro [x y] 1 2))
;; (x 3 4)
;; x
;; (print foo)
;; foo
;; 234
;; (defmacro foo [x] `(print ~x))
;; (foo x)
;; x
;; (def sym 1)
;; (def params [1 2])
;; (def body  [1 2 3])
;; `(~@body)
;; `(~@(list 1))


;; (def foo (macro [x] (list 'pr x)))
;; (def foo (macro [x] x))
;; (def bar (macro [y] (print 123) (list 'foo y)))

;; (foo "WITHOUT QUOTES?" )
;; (bar "WITHOUT QUOTES?" )
;; (macroexpand '(foo 123))
;; 1

