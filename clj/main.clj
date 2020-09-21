(in-ns 'main)
;; (require '[test.run :refer [run-tests]])
;; (clojure.core/use 'clojure.core)
;; (require '[foo.core :as foo])

;; (require '[foo.test4-if-fn-do :as foo ;; :refer [foo]
;;            ])
;; (require '[bar.core :as bar :refer [bar]])

;; (def a 123)
;; (def b 123)
;; (defn bar[x] x)
;; (defn foo [x] x)
;; (def fox bar)
;; (def list-with-bar (list bar))
;; (def bar2 bar)
;; (print "hello")
;; (defn q [])
;; (defn f [x y & z] [x y z])
;; (defn g [x y z a] (list x y (f z a)))
(defn main [x y]
  (let [p +
        ;; f (fn [x y] [x y])
        ]
    ;; (print (+ 1 1) (p 1 1))
    (p 1 1)
    )
  ;; (let [f (fn [x y] [x y])]
  ;;   (print (f 1 2)))
  ;; (let [h (fn [x y] [x y])]
  ;;   ;; (let [a 1]
  ;;   ;;   (print "hello" a x y))
  ;;   ;; (print (h 123 456 999) ;; (g 1 2 3 4)
  ;;   ;;        )
  ;;   )
  ;; (f 1)
  ;; (foo/foo 1)
  ;; (foo/foo2 1)
  ;; (let [g (fn [x] x)
  ;;       a 123]
  ;;   (print "hello" (f 1 2 3) (g 2) g f ;; foo/foo
  ;;          ))
  )
;; (main 1 2)
;; (defn g [
;;          k0 k1 k2 k3 k4 k5 k6 k7 k8 k9
;;          k10 k11 k12 k13 k14 k15 k16 k17 k18 & k19
;;          ] (print k0 k19))
;; (g 0 1 2 3 4 5 6 7 8 9
;;    10 11 12 13 14 15 16 17 18 19 20 21
;;    )


;; (print (read-string ":kw"))
;; (print (first (read-string "(1 2 3)")))
;; (pr (str "line1\nline2: foo
;; line3" 1 (list 1 2) :kw 'sym))

;; (pr "line1\nline2: foo
;; line3")
;; (print (str "abcd"))
;; (run-tests)
;; (foo/t)
;; (foo/foofoofoo)
;; (foo/baabaabaabaabaa)

;; (foo/foo2)
;; (foo/baa2)
;; (foo/t)

;; (t ( (fn [a b] (+ b a)) 3 4) 7)
;; (t ( (fn [] 4) ) 4)

;; (t (+ 5 (* 2 3))
;;    11)

;; (t (- (+ 5 (* 2 3)) 3)
;;    8)

;; (t (/ (- (+ 5 (* 2 3)) 3) 4)
;;    2)

;; (t (/ (- (+ 515 (* 87 311)) 302) 27)
;;    1010)

;; (t (* -3 6)
;;    -18)
;; (pr "FAIL" "foo")
;; (foo 1)

;; (print (quote
;;         ;; (

;;         ;;  0 1 2 3 4 5 6 7 8 9
;;         ;;  0 1 2 3 4 5 6 7 8 9
;;         ;;  0 1 2 3 4 5 6 7 8 9

;;         ;;  ;; 0 1 2 3 4 5 6 7 8 9
;;         ;;  ;; 0 1 2 3 4 5 6 7 8 9
;;         ;;  )
;;         (list 112 113 114 115 116 117 118 119 11
;;               12 13 14 15 16 17 18 19 1 2 3 4 5
;;               6 7 8 9
;;               112 113 114 115 116 117 118 119 11
;;               12 13 14 15 16 17 18 19 1 2 3 4 5
;;               6 7 8 9

;;         )
;;        )
;;       )

  ;; (t r 1010)

  
  ;; (t (+ 1 2 3 4 5 6 7 8 9)
  ;;    1010)
  
  ;; (t (+ 11 12 13 14 15 16 17 18 19)
  ;;    1010)
  ;; (t (* -3 6)
  ;;    -18)

  ;; (if false 1 )
  ;; (if false 1)
  ;; "foo"
  ;; (t (/ (- (+ 515 (* 87 311)) 302) 27)
  ;;    1010)
  ;; (test2/test)
  ;; (if (+ 1 1) 1 2)
  ;; (if true (+ 1 1))
  ;; (if false 1)
  ;; "foo"

  ;; (let [evalled 1]
  ;;   (when (not= evalled (quote 1))
  ;;     (do
  ;;       (pr "FAIL" "foo")
  ;;       (pr (quote 1) )
  ;;       (pr "Expected:" (quote 1))
  ;;       (pr "Got:" 1))))

  ;; (t 1 1)

  ;; (let [evalled 1]
  ;;   (when (not= evalled (quote 1))
  ;;     (do
  ;;       (pr "FAIL" "foo")
  ;;       (pr (quote 1) )
  ;;       (pr "Expected:" (quote 1))
  ;;       (pr "Got:" 1))))
 
  ;; (t 1 2)
  ;; "foo"

  ;; (if 1 false)
  ;; (if (f 1) false)
  ;; bar.core/bar
  ;; fox
  ;; bar.core/bar
  ;; (print "main/test--------------")
  ;; (print "bar.core/bar: " bar.core/bar)
  ;; (print "fox: " bar/fox)
  ;; (print "bar: " bar)
  ;; (print "bar: " bar/bar)
  ;; (print list-with-bar +)
  ;; (print +)
  ;; (print bar)
  ;; (print (list bar))

  ;; (print "bar.core/bar: " bar.core/bar)
 


  ;; (print "foo: " bar bax)
  ;; (print "calling test2/test")
  ;; (test2/test)
  ;; (print "done---------------")
  ;; (test2/test)
  ;; (print (foo/f 456))
  ;; (let [f foo/f]
  ;;   (print "hello" (f 123) (foo/f 456)
  ;;          ))
  ;; (print "hello" a b (foo 4444) bar)
  ;; (print "hello there" (foo 555))
  ;; (print "hello", 123, 345, false, true, x y (list 1 2) "some string")
  ;; (print foo.core/foo foo/foo foo foo/f a b)
  ;; (foo/f) foo2/foo2
  ;; (foo/f)
  ;; a foo.core/foo foo/foo foo

  ;; (print "foo.core/foo = " foo.core/foo)
  ;; (print "foo/foo = " foo/foo)
  ;; (print "foo" foo)
  ;; (let [f foo
  ;;       p print]
  ;;   (p "foobar" "hello" :kw 'symbol '(1 2 3 4 5 ) (foo) (f)))
  ;; (print 123 456 999 "foo")
  ;; (print 12345 a (foo))

;; (compile "lispy/test_compile.lispy")


;; (def plus +)
;; (def mylet (if true let))
;; (defmacro my-macro2 [x] `(plus 1 ~x))
;; (defmacro my-macro [x] `(plus 1 ~x (my-macro2 1)
;;                               ))

;; (defn foo [x & rest] (print "from foo" x rest))
;; (defn foo [x] 1)
;; (def v [ 1 2 3 ])

;; (defn test2 [x]
;;   (let [a (f "abc" Rl(+ 1 1) 1 Rl(+ 1 x) x)
;;         a1 (f a 1)
;;         f2 ([y] y)
;;         c "abc"
;;         d v]
;;     "abc" c Rl(+ 1 1)
;;    rt b

;;     )
;;   )
;; (defn foo [foo-x y] foo-x)
;; (defn bar [x] (print x) ;; (+ 1 1)
;;   )
;; (def z 123)

;; (def b 123)

;; (def c 567)

;; (let [b 777]
;;   (def a (list (fn [] '(1 2 3 5))))
;;   )

;; (defn foo [x y & z] x)
;; (def plus +)
;; (defn foo [x y ] 'from-foo)
;; (defmacro t2 [expr expected & str]
;;   `(let [evalled ~expr]
;;      (print evalled)
;;      (print (quote ~expected))
;;      (when (not= evalled  ~expected)
;;       (do
;;         (pr "FAIL" ~@str)
;;         (pr (quote ~expr) )
;;         (pr "Expected:" ~expected)
;;         (pr "Got:" ~expr)
;;        )
;;      )))
;; (defn bar[] 123)
;; (def bax (list 1 2 3 4 5))
;; (defn baq [x y & z] (list 1 2 3 5))

;; (def bar (list (fn [] 123)))
;; (def a (list (fn [] 123)))
;; (def bar (fn [] 555))

;; (in-ns 'my.namespace)
;; (defn test [x y]
;;  123
;;   ;; (print 'abc 'abc)
;;   ;; (print "hello" 456 (foo))
;;   ;; (let [f (fn [x] 123)
;;   ;;       g foo
;;   ;;       h bar]
;;   ;;   ;; (print "hello!!!")
;;   ;;   ;; (print 'abc 'abc)
;;   ;;   (h) (g) (h) (print bax))
;;   ;; (print "abc" "abc")
;;   ;; (foo 1 2 3)
;; ;; + +
;; ;;   a a
;;   ;; bar bar
;;   ;; a a
;;   ;; bar
;;   ;; (print ((first bar)))
;;   ;; (let [f (f)]
;;   ;;   (print (f))
;;   ;;   ;; a
;;   ;;   ;; 123
;;   ;;   ;; (print a a)
;;   ;;   )
;;   ;; (print a)
;;   ;; (if nil (print 1) (print 0))
;;   ;; (let [a (foo)]
;;   ;;   1 a)
;;   ;; (do 1 123)
;;   ;; (t2 (+ 1 1) (- 3 1) "should be equal")
;;   ;; (let [evalled (+ 1 1)]
;;   ;;   (print evalled)
;;   ;;    (if (= 2  3)
;;   ;;      (print "equal")
;;   ;;      (print "not equal")

;;   ;;    ))
;;   ;; (print (my-macro 1))
;;   ;; (t (list 1 2 3) (1 2 3))
;;   ;; (let [f foo]

;;   ;; (print '(1 (fn [x] x) 3))
;;   ;;   (print f)
;;   ;;   ;; (print (list (fn [x] x) ))
;;   ;;   )
;;   ;; (print   ((first a)) )
;;   ;; (let [f foo
;;   ;;       f2 +]
;;   ;;   ;; (f 123)
;;   ;;   )
;;   ;; (let [v (list (fn [x] x))]
;;   ;;   (print  v)
;;   ;;   ;; (f 1 )
;;   ;;   )
;;   ;; (print a)
;;   ;; (print a)
;;   ;; x y
;;   ;; (let [a 1 ] (bar 1))
;;   ;; (let [f? (fn [x] x)] (f? 1))
;;   ;; (let [a (foo 12 3) d (+ 1 2) b (bar 1) e 1] b e)
;;   ;; (foo (bar 1) (bar 1))
;;   ;; (let [x 1] x)
;;   ;; (let [x 1] x)
;;   ;; (do 1 (bar 1) 2)
;;   ;; (let [a (bar 1) a (+ a 1)] (print a))
;;   ;; (foo (bar 1) (bar 1))
;;   ;; (bar (foo "hello"))
;;   ;; (let [f foo
;;   ;;       g (fn [x] x)]
;;   ;;   (print (+ 1 1) (f 123) (f 234)))
;;   ;; (print (quote (1 2)))
;;   ;; (do 1 1)
;;   ;; 1
;;   ;; (let [x 1]
;;   ;;   (if x nil ))
;;   ;; (print (if true 1 2))
;;   ;; (print "Testing!!!")
;;   ;; (print "from test")



;;   ;; (mylet [x plus
;;   ;;         y plus]
;;   ;;        (print x y))
;;   ;; (when true (print "when macro!!!"))
;;   ;; (let [p print]
;;   ;;   (p (my-macro 1)))
;;   ;;  (let [f foo]
;;   ;;    (f 123 1 2 3)
;;   ;;    ;; (print "from test")
;;   ;;    ;; (f)

;;   ;; )
;;   )

;; (def foo 123)
;; (defn test [a a1]
;;   (let [z 123
;;         f (fn [b] (let [c 99 d 99]  a a1 b c z
;;                     ))]
;;     1))
;; (def a 1)
;; (defn f [x]
;;   ;; (print "In f!!!!!" x)
;;   (let [q (fn [y] (print "HELLO" x y) )]
;;     ;; (q 777)
;;     q
;;     )
;;   )

;; (defn test [x y]
;;   ;; (print  123 x y)
;;   (let [g (f 123)]
;;     (print "G: " g)
;;     (g 456)
;;     ;; 123
;;     )
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
;;                 (fn [x y] (list a b x y ))))
;; (defn adder [x]
;;   (fn [y] (+ x y)))
;; (defn foo [a]
;;   ;; (print "a b: " a b)
;;   ;; (print "rest: " rest)
;;  ;; 1
;;   (print  a)
;;   )


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

