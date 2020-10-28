(in-ns 'test.test4)


(defn test4a []
  ;; ;; Testing list functions
  (t (list) ())
  ;;TODO TEST
  (t (list? (list)) true)
 
  ;; (t (empty? (list)) true)
  ;; (t (empty? (list 1)) false)

  (t (list 1 2 3) (1 2 3))
  (t (count (list 1 2 3)) 3)
  (t (count (list)) 0)
 
  ;; (t (count nil) 0)
  (t (if (> (count (list 1 2 3)) 3) 89 78) 78)
  (t (if (>= (count (list 1 2 3)) 3) 89 78) 89)



  ;; ;; ;; Testing if form
  (t (if true 7 8) 7)
  (t (if false 7 8) 8)
  (t (if false 7 false) false)
  (t (if true (+ 1 7) (+ 1 8)) 8)
  (t (if false (+ 1 7) (+ 1 8)) 9)
  (t (if nil 7 8) 8)
  (t (if 0 7 8) 7)
  (t (if (list) 7 8) 7)
  (t (if (list 1 2 3) 7 8) 7)
  (t (= (list) nil) false)


  ;; ;; ;; Testing 1-way if form
  (t (if false (+ 1 7)) nil)
  (t (if nil 8) nil)
  (t (if nil 8 7) 7)
  (t (if true (+ 1 7)) 8)

  ;; ;; ;; Testing basic conditionals
  (t (= 2 1)
     false)
  (t (= 1 1)
     true)
  (t (= 1 2)
     false)
  (t (= 1 (+ 1 1))
     false)
  (t (= 2 (+ 1 1))
     true)
  (t (= nil 1)
     false)
  (t (= nil nil)
     true)

  (t (> 2 1)
     true)
  (t (> 1 1)
     false)
  (t (> 1 2)
     false)

  (t (>= 2 1)
     true)
  (t (>= 1 1)
     true)
  (t (>= 1 2)
     false)

  (t (< 2 1)
     false)
  (t (< 1 1)
     false)
  (t (< 1 2)
     true)

  (t (<= 2 1)
     false)
  (t (<= 1 1)
     true)
  (t (<= 1 2)
     true)

  ;; ;; ;; Testing equality
  (t (= 1 1)
     true)
  (t (= 0 0)
     true)
  (t (= 1 0)
     false)
  (t (= true true)
     true)
  (t (= false false)
     true)
  (t (= nil nil)
     true)

  (t (= (list) (list))
     true)
  (t (= (list 1 2) (list 1 2))
     true)
  (t (= (list 1) (list))
     false)
  (t (= (list) (list 1))
     false)
  (t (= 0 (list))
     false)
  (t (= (list) 0)
     false)
  (t (= (list nil) (list))
     false)


  ;; ;; ;; Testing builtin and user defined functions
  (t (+ 1 2) 3)
  (t ( (fn [a b] (+ b a)) 3 4) 7)
  (t ( (fn [] 4) ) 4)

  (t ((fn [f x] (f x)) (fn [a] (+ 1 a)) 7) 8)


  ;; ;; Testing closures
  ;; (t ( ( (fn [a] (fn [b] (+ a b))) 5) 7) 12)
  ;; (t 1 2)
  )

(def gen-plus5 (fn [] (fn [b] (+ 5 b))))
(def plus5 (gen-plus5))

(def gen-plusX (fn [x] (fn [b] (+ x b))))
(def plus7 (gen-plusX 7))
(do (def a 6) 7 (+ a 8))

;; TODO: use let to make the closed over fns
(defn test4b []

  (t (plus5 7) 12)
  (t (plus7 8) 15)

  ;; Testing do form
  ;; (t (do (pr 101))
  ;;    nil)
  (t (do (pr 102) 7)
     7)
  (t (do (pr 101) (pr 102) (+ 1 2))
     3)

  (t a 6)
  

  ;; ;; Testing special form case-sensitivity
  ;; (def DO (fn [a] 7))
  ;; (t (DO 3)
  ;;    7)


  ;; ;; ;; Testing recursive sumdown function
  ;; (def sumdown (fn [N] (if (> N 0) (+ N (sumdown  (- N 1))) 0)))
  ;; (t (sumdown 1) 1)
  ;; (t (sumdown 2) 3)
  ;; (t (sumdown 6) 21)

  ;; ;; Testing recursive fibonacci function
  ;; (def fib (fn [N] (if (= N 0) 1 (if (= N 1) 1 (+ (fib (- N 1)) (fib (- N 2)))))))
  ;; (t (fib 1)
  ;;    1)
  ;; (t (fib 2)
  ;;    2)
  ;; (t (fib 4)
  ;;    5)



  ;; ;; Testing recursive function in environment.

  ;; This fails in clojure, so also in lispy: unknown symbol cst. env gets built
  ;; as each assignment is done, and used as closure for each _next_ assigned
  ;; value.
  ;; (t (let [cst (fn [n] (if (= n 0) 0 (cst (- n 1))))] (cst 1))
  ;;    0)
  ;; (t (let [f (fn [n] (if (= n 0) 0 (g (- n 1)))) ;; so you can't call forward references!!!
  ;;          g (fn [n] (f n))]
  ;;      (f 2))
  ;;    0)


  ;; ;; Testing if on strings

  (if "" 7 8))
                                        ;=>7

(defn test4c []
  ;; Testing string equality

  (t (= "" "")
     true)
  (t (= "abc" "abc")
     true)
  (t (= "abc" "")
     false)
  (t (= "" "abc")
     false)
  (t (= "abc" "def")
     false)
  (t (= "abc" "ABC")
     false)
  (t (= (list) "")
     false)
  (t (= "" (list))
     false)

  ;; Testing variable length arguments

  (t ( (fn [& more] (count more)) 1 2 3)
     3)
  (t ( (fn [& more] (list? more)) 1 2 3)
     true)
  (t ( (fn [& more] (count more)) 1)
     1)
  (t ( (fn [& more] (count more)) )
     0)
  (t ( (fn [& more] (list? more)) )
     false)
  (t ( (fn [a & more] (count more)) 1 2 3)
     2)
  (t ( (fn [a & more] (count more)) 1)
     0)
  (t ( (fn [a & more] (list? more)) 1)
     false)

  ;; Testing language defined not function
  ;; (t (not false)
  ;;    true)
  ;; (t (not nil)
  ;;    true)
  ;; (t (not true)
  ;;    false)
  ;; (t (not "a")
  ;;    false)
  ;; (t (not 0)
  ;;    false)
  )


;; ;; -----------------------------------------------------

(defn test4d []
  ;; ;; Testing string quoting

  (t ""
     "")

  (t "abc"
     "abc")

  (t "abc  def"
     "abc  def")

  (t "\""
     "\"")

  (t "abc\ndef\nghi"
     "abc\ndef\nghi")

  (t "abc\\def\\ghi"
     "abc\\def\\ghi")

  (t "\\n"
     "\\n")

  ;; ;; Testing pr-str

  ;; ;; (t (pr-str)
  ;; ;;    "")

  ;; ;; (t (pr-str "")
  ;; ;;    "\"\"")

  ;; ;; (t (pr-str "abc")
  ;; ;;    "\"abc\"")

  ;; ;; (t (pr-str "abc  def" "ghi jkl")
  ;; ;;    "\"abc  def\" \"ghi jkl\"")

  ;; ;; (t (pr-str "\"")
  ;; ;;    "\"\\\"\"")

  ;; ;; (t (pr-str (list 1 2 "abc" "\"") "def")
  ;; ;;    "(1 2 \"abc\" \"\\\"\") \"def\"")

  ;; ;; (t (pr-str "abc\ndef\nghi")
  ;; ;;    "\"abc\\ndef\\nghi\"")

  ;; ;; (t (pr-str "abc\\def\\ghi")
  ;; ;;    "\"abc\\\\def\\\\ghi\"")

  ;; ;; (t (pr-str (list))
  ;; ;;    "()")

  ;; ;; Testing str

  ;; ;; (t (str)
  ;; ;;    "")

  ;; ;; (t (str "")
  ;; ;;    "")

  ;; ;; (t (str "abc")
  ;; ;;    "abc")

  ;; ;; (t (str "\"")
  ;; ;;    "\"")

  ;; ;; (t (str 1 "abc" 3)
  ;; ;;    "1abc3")

  ;; ;; (t (str "abc  def" "ghi jkl")
  ;; ;;    "abc  defghi jkl")

  ;; ;; (t (str "abc\ndef\nghi")
  ;; ;;    "abc\ndef\nghi")

  ;; ;; (t (str "abc\\def\\ghi")
  ;; ;;    "abc\\def\\ghi")

  ;; ;; (t (str (list 1 2 "abc" "\"") "def")
  ;; ;;    "(1 2 abc \")def")

  ;; ;; (t (str (list))
  ;; ;;    "()")

  ;; ;; Testing prn
  ;; ;; (t (prn)

  ;; ;;                                         ;=>nil

  ;; ;;    (prn ""))
  ;; ;/""
  ;; ;=>nil

  ;; ;; (t (prn "abc")
  ;; ;;    "abc")
  ;; ;=>nil

  ;; ;; (t (prn "abc  def" "ghi jkl")
  ;; ;;    "abc  def")
  ;; "ghi jkl"

  ;; ;; (t (t (prn "\""))
  ;; ;;    "=>nil
  ;; ;; /")
  ;; ;; \\"

  ;; ;; (t (prn "abc\ndef\nghi")
  ;; ;;    nil)
  ;; ;/"abc\\ndef\\nghi"

  ;; ;; (t (prn "abc\\def\\ghi")
  ;; ;;    nil)
  ;; ;/"abc\\\\def\\\\ghi"

  ;; ;; (t (prn (list 1 2 "abc" "\"") "def")
  ;; ;;    nil)
  ;; ;/\(1 2 "abc" "\\""\) "def"


  ;; ;; Testing println
  ;; ;; (t (println)
  ;; ;;    nil)
  ;; ;/


  ;; ;; (t (println "")
  ;; ;;    nil)

  ;; ;/

  ;; ;; (t (println "abc")
  ;; ;;    nil)

  ;; ;/abc

  ;; ;; (println "abc  def" "ghi jkl")
  ;; ;/abc  def ghi jkl

  ;; ;; (t (println "\"")
  ;; ;;    nil)
  ;; ;/"

  ;; ;; (println "abc\ndef\nghi")
  ;; ;/abc
  ;; ;/def
  ;; ;/ghi
  ;; ;=>nil

  ;; ;; (println "abc\\def\\ghi")
  ;; ;/abc\\def\\ghi
  ;; ;=>nil

  ;; ;; (println (list 1 2 "abc" "\"") "def")
  ;; ;/\(1 2 abc "\) def
  ;; ;=>nil


  )


(defn test4e []
  ;; Testing keywords
  (t (= :abc :abc)
     true)
  (t (= :abc :def)
     false)
  (t (= :abc ":abc")
     false)
  (t (= (list :abc) (list :abc))
     true)

  ;; ;; Testing vector truthiness
  (t (if [] 7 8)
     7)

  ;; ;; Testing vector printing
  ;; ;; (pr-str [1 2 "abc" "\""] "def")
  ;; ;=>"[1 2 \"abc\" \"\\\"\"] \"def\""

  ;; ;; (pr-str [])
  ;; ;=>"[]"

  ;; ;; (str [1 2 "abc" "\""] "def")
  ;; ;=>"[1 2 abc \"]def"

  ;; ;; (t (str [])
  ;; ;;    "[]")


  ;; ;; Testing vector functions
  (t (count [1 2 3])
     3)
  ;; (t (empty? [1 2 3])
  ;;    false)
  ;; (t (empty? [])
  ;;    true)
  ;; (t (list? [4 5 6])
  ;;    false)

  ;; ;; Testing vector equality
  (t (= [] (list))
     true)
  (t (= [7 8] [7 8])
     true)
  ;; ;; (t (= [:abc] [:abc])
  ;; ;;    true)
  (t (= (list 1 2) [1 2])
     true)
  (t (= (list 1) [])
     false)
  (t (= [] [1])
     false)
  (t (= 0 [])
     false)
  (t (= [] 0)
     false)
  (t (= [] "")
     false)
  (t (= "" [])
     false)

  ;; ;; Testing vector parameter lists
  (t ( (fn [] 4) )
     4)
  (t ( (fn [f x] (f x)) (fn [a] (+ 1 a)) 7)
     8)

  ;; ;; Nested vector/list equality
  (t (= [(list)] (list []))
     true)
  (t (= [1 2 (list 3 4 [5 6])] (list 1 2 [3 4 (list 5 6)]))
     true))
;; "done"



