(in-ns 'test.test9-try)
;;
;; Testing throw

;; (t (throw "err1") "err1")
;/.*([Ee][Rr][Rr][Oo][Rr]|[Ee]xception).*err1.*

;;
;; Testing try/catch E

(t (try 123 (catch E e 456))
   123)

;; (try abc (catch E exc (prn "exc is:" exc)))
;;    "exc is:" "'abc' not found"
;=>nil

;; (try (abc 1 2) (catch E exc (prn "exc is:" exc)))
;/"exc is:" "'abc' not found"
;=>nil

;; Make sure error from core can be caught
;; (try (nth () 1) (catch E exc (prn "exc is:" exc)))
;/"exc is:".*(length|range|[Bb]ounds|beyond).*
;=>nil

;; (try (throw "my exception") (catch E exc (do (prn "exc:" exc) 7)))
;/"exc:" "my exception"
;=>7

;;; Test that throw is a function:
;; (try (map throw (list "my err")) (catch E exc exc))
;=>"my err"


;;
;; Testing builtin functions

;; (t (symbol? 'abc)
;;    true)
;; (t (symbol? "abc")
;;    false)

;; (t (nil? nil)
;;    true)
;; (t (nil? true)
;;    false)

;; (t (true? true)
;;    true)
;; (t (true? false)
;;    false)
;; (t (true? true?)
;;    false)

;; (t (false? false)
;;    true)
;; (t (false? true)
;;    false)

;; ;; Testing apply function with core functions
;; (t (apply + (list 2 3))
;;    5)
;; (t (apply + 4 (list 5))
;;    9)
;; (t (apply prn (list 1 2 "3" (list)))
;;    2) "3" \(\)
;; ;=>nil
;; (t (apply prn 1 2 (list "3" (list)))
;;    2) "3" \(\)
;; ;=>nil
;; (t (apply list (list))
;;    ())
;; (t (apply symbol? (list (quote two)))
;;    true)

;; ;; Testing apply function with user functions
;; (t (apply (fn (a b) (+ a b)) (list 2 3))
;;    5)
;; (t (apply (fn (a b) (+ a b)) 4 (list 5))
;;    9)

;; ;; Testing map function
;; (def! nums (list 1 2 3))
;; (def! double (fn (a) (* 2 a)))
;; (t (double 3)
;;    6)
;; (t (map double nums)
;;    (2 4 6))
;; (t (map (fn (x) (symbol? x)) (list 1 (quote two) "three"))
;;    (false true false))

;; ;; Testing throwing a hash-map
;; (t (throw {:msg "err2"})
;;    *)([Ee][Rr][Rr][Oo][Rr]|[Ee]xception).*msg.*err2.*

;; ;; Testing symbol and keyword functions
;; (t (symbol? :abc)
;;    false)
;; (t (symbol? 'abc)
;;    true)
;; (t (symbol? "abc")
;;    false)
;; (t (symbol? (symbol "abc"))
;;    true)
;; (t (keyword? :abc)
;;    true)
;; (t (keyword? 'abc)
;;    false)
;; (t (keyword? "abc")
;;    false)
;; (t (keyword? "")
;;    false)
;; (t (keyword? (keyword "abc"))
;;    true)

;; (t (symbol "abc")
;;    abc)
;; (t (keyword "abc")
;;    :abc)

;; ;; Testing sequential? function

;; (t (sequential? (list 1 2 3))
;;    true)
;; (t (sequential? [15])
;;    true)
;; (t (sequential? sequential?)
;;    false)
;; (t (sequential? nil)
;;    false)
;; (t (sequential? "abc")
;;    false)

;; ;; Testing apply function with core functions and arguments in vector
;; (t (apply + 4 [5])
;;    9)
;; ;; (apply prn 1 2 ["3" 4])
;; ;/1 2 "3" 4
;; ;=>nil
;; (t (apply list [])
;;    ())
;; ;; Testing apply function with user functions and arguments in vector
;; (t (apply (fn (a b) (+ a b)) [2 3])
;;    5)
;; (t (apply (fn (a b) (+ a b)) 4 [5])
;;    9)


;; ;; Testing map function with vectors
;; (t (map (fn (a) (* 2 a)) [1 2 3])
;;    (2 4 6))

;; (t (map (fn [& args] (list? args)) [1 2])
;;    (true true))

;; ;; Testing vector functions

;; (t (vector? [10 11])
;;    true)
;; (t (vector? '(12 13))
;;    false)
;; (t (vector 3 4 5)
;;    [3 4 5])

;; (t (map? {})
;;    true)
;; (t (map? '())
;;    false)
;; (t (map? [])
;;    false)
;; (t (map? 'abc)
;;    false)
;; (t (map? :abc)
;;    false)


;; ;;
;; ;; Testing hash-maps
;; (t (hash-map "a" 1)
;;    {"a" 1})

;; (t {"a" 1}
;;    {"a" 1})

;; (t (assoc {} "a" 1)
;;    {"a" 1})

;; (t (get (assoc (assoc {"a" 1 } "b" 2) "c" 3) "a")
;;    1)

;; (def! hm1 (hash-map))
;; ;=>{}

;; (t (map? hm1)
;;    true)
;; (map? 1)
;; ;=>false
;; (t (map? "abc")
;;    false)

;; (t (get nil "a")
;;    nil)

;; (t (get hm1 "a")
;;    nil)

;; (t (contains? hm1 "a")
;;    false)

;; (def! hm2 (assoc hm1 "a" 1))
;; ;=>{"a" 1}

;; (t (get hm1 "a")
;;    nil)

;; (t (contains? hm1 "a")
;;    false)

;; (t (get hm2 "a")
;;    1)

;; (t (contains? hm2 "a")
;;    true)


;; ;;; TODO: fix. Clojure returns nil but this breaks mal impl
;; (t (keys hm1)
;;    ())

;; (t (keys hm2)
;;    ("a"))

;; (t (keys {"1" 1})
;;    ("1"))

;; ;;; TODO: fix. Clojure returns nil but this breaks mal impl
;; (t (vals hm1)
;;    ())

;; (t (vals hm2)
;;    (1))

;; (t (count (keys (assoc hm2 "b" 2 "c" 3)))
;;    3)

;; ;; Testing keywords as hash-map keys
;; (t (get {:abc 123} :abc)
;;    123)
;; (t (contains? {:abc 123} :abc)
;;    true)
;; (t (contains? {:abcd 123} :abc)
;;    false)
;; (t (assoc {} :bcd 234)
;;    {:bcd 234})
;; (t (keyword? (nth (keys {:abc 123 :def 456}) 0))
;;    true)
;; (t (keyword? (nth (vals {"a" :abc "b" :def}) 0))
;;    true)

;; ;; Testing whether assoc updates properly
;; (def! hm4 (assoc {:a 1 :b 2} :a 3 :c 1))
;; (t (get hm4 :a)
;;    3)
;; (t (get hm4 :b)
;;    2)
;; (t (get hm4 :c)
;;    1)

;; ;; Testing nil as hash-map values
;; (t (contains? {:abc nil} :abc)
;;    true)
;; (t (assoc {} :bcd nil)
;;    {:bcd nil})

;; ;;
;; ;; Additional str and pr-str tests

;; (t (str "A" {:abc "val"} "Z")
;;    "A{:abc val}Z")

;; (t (str true "." false "." nil "." :keyw "." 'symb)
;;    "true.false.nil.:keyw.symb")

;; ;; (pr-str "A" {:abc "val"} "Z")
;; ;=>"\"A\" {:abc \"val\"} \"Z\""

;; ;; (pr-str true "." false "." nil "." :keyw "." 'symb)
;; ;=>"true \".\" false \".\" nil \".\" :keyw \".\" symb"

;; (def! s (str {:abc "val1" :def "val2"}))
;; (t (cond (= s "{:abc val1 :def val2}") true (= s "{:def val2 :abc val1}") true)
;;    true)

;; (def! p (pr-str {:abc "val1" :def "val2"}))
;; (t (cond (= p "{:abc \"val1\" :def \"val2\"}") true (= p "{:def \"val2\" :abc \"val1\"}") true)
;;    true)

;; ;;
;; ;; Test extra function arguments as Mal List (bypassing TCO with apply)
;; (t (apply (fn (& more) (list? more)) [1 2 3])
;;    true)
;; (t (apply (fn (& more) (list? more)) [])
;;    true)
;; (t (apply (fn (a & more) (list? more)) [1])
;;    true)

;; ;;;TODO: fix so long lines don't trigger ANSI escape codes ;;;(try
;; ;;;(try (throw ["data" "foo"]) (catch E exc (do (prn "exc is:" exc) 7))) ;;;;
;; ;;;; "exc is:" ["data" "foo"] ;;;;=>7
;; ;;;;=>7

;; ;;
;; ;; Testing try without catch E
;; ;; (try xyz)
;; ;/.*\'?xyz\'? not found.*

;; ;;
;; ;; Testing throwing non-strings
;; ;; (try (throw (list 1 2 3)) (catch E exc (do (prn "err:" exc) 7)))
;; ;/"err:" \(1 2 3\)
;; ;=>7

;; ;;
;; ;; Testing dissoc
;; (def! hm3 (assoc hm2 "b" 2))
;; (t (count (keys hm3))
;;    2)
;; (t (count (vals hm3))
;;    2)
;; (t (dissoc hm3 "a")
;;    {"b" 2})
;; (t (dissoc hm3 "a" "b")
;;    {})
;; (t (dissoc hm3 "a" "b" "c")
;;    {})
;; (t (count (keys hm3))
;;    2)

;; (t (dissoc {:cde 345 :fgh 456} :cde)
;;    {:fgh 456})
;; (t (dissoc {:cde nil :fgh 456} :cde)
;;    {:fgh 456})

;; ;;
;; ;; Testing equality of hash-maps
;; (t (= {} {})
;;    true)
;; (t (= {:a 11 :b 22} (hash-map :b 22 :a 11))
;;    true)
;; (t (= {:a 11 :b [22 33]} (hash-map :b [22 33] :a 11))
;;    true)
;; (t (= {:a 11 :b {:c 33}} (hash-map :b {:c 33} :a 11))
;;    true)
;; (t (= {:a 11 :b 22} (hash-map :b 23 :a 11))
;;    false)
;; (t (= {:a 11 :b 22} (hash-map :a 11))
;;    false)
;; (t (= {:a [11 22]} {:a (list 11 22)})
;;    true)
;; (t (= {:a 11 :b 22} (list :a 11 :b 22))
;;    false)
;; (t (= {} [])
;;    false)
;; (t (= [] {})
;;    false)

;; (t (keyword :abc)
;;    :abc)
;; (t (keyword? (first (keys {":abc" 123 ":def" 456})))
;;    false)
