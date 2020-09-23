(in-ns 'test.test6-file)
;;; TODO: really a step5 test
;;

(defn test []
  ;; Testing that (do (do)) not broken by TCO
  (t (do (do 1 2))
     2)

  ;;
  ;; Testing read-string, eval and slurp
  ;; (t (read-string "(1 2 (3 4) nil)")
  ;;    (1 2 (3 4) nil))

  ;; (t (= nil (read-string "nil"))
  ;;    true)

  ;; (t (read-string "(+ 2 3)")
  ;;    (+ 2 3))

  ;; (t (read-string "\"\n\"")
  ;;    "\n")

  ;; (t (read-string "7 ;; comment")
  ;;    7)

;;; Differing output, but make sure no fatal error
  ;; (read-string ";; comment") ;;TODO: failing


  ;;    (eval (read-string "(+ 2 3)")))
                                        ;=>5

  ;; (t (slurp "../tests/test.txt")
  ;;    "A line of text\n")

;;; Load the same file twice.
  ;; (t (slurp "../tests/test.txt")
  ;;    "A line of text\n")

  ;; Testing load-file

  ;; (t (load-file "../tests/inc.mal")
  ;;    nil)
  ;; (t (inc1 7)
  ;;    8)
  ;; (t (inc2 7)
  ;;    9)
  ;; (t (inc3 9)
  ;;    12)

  ;;
  ;; Testing atoms

  ;; (t (def! inc3 (fn* (a) (+ 3 a)))

  ;;    (def! a (atom 2)))
                                        ;=>(atom 2)

  ;; (t (atom? a)
  ;;    true)

  ;; (t (atom? 1)
  ;;    false)

  ;; (t (deref a)
  ;;    2)

  ;; (t (reset! a 3)
  ;;    3)

  ;; (t (deref a)
  ;;    3)

  ;; (t (swap! a inc3)
  ;;    6)

  ;; (t (deref a)
  ;;    6)

  ;; (t (swap! a (fn* (a) a))
  ;;    6)

  ;; (t (swap! a (fn* (a) (* 2 a)))
  ;;    12)

  ;; (t (swap! a (fn* (a b) (* a b)) 10)
  ;;    120)

  ;; (t (swap! a + 3)
  ;;    123)

  ;; Testing swap!/closure interaction
  ;; (t (def! inc-it (fn* (a) (+ 1 a)))
  ;;    f!)
  ;; atm ;; (atom 7)
  ;; (t (def! f (fn* () (swap! atm inc-it)))

  ;;                                         ;=>8
  ;;    (f))
                                        ;=>9

  ;; Testing whether closures can retain atoms
  ;; (t (def! g (let* (atm (atom 0)) (fn* () (deref atm))))
  ;;    f!) atm (atom 1)
  ;; (t (g)
  ;;    0)

                                        ;>>> deferrable=True
  ;;
  ;; -------- Deferrable Functionality --------

  ;; Testing reading of large files
  ;; (t (load-file "../tests/computations.mal")
  ;;    nil)
  ;; (t (sumdown 2)
  ;;    3)
  ;; (t (fib 2)
  ;;    1)

  ;; Testing `@` reader macro (short for `deref`)
  ;; (t (def! atm (atom 9))
  ;;    m)
                                        ;=>9

;;; TODO: really a step5 test
  ;; Testing that vector params not broken by TCO
  ;; (t (def! g (fn* [] 78))

  ;;                                         ;=>78
  ;;    (def! g (fn* [a] (+ a 78))))
  ;; (t (g 3)
  ;;    81)

  ;;
  ;; Testing that *ARGV* exists and is an empty list
  ;; (t (list? *ARGV*)
  ;;    true)
  ;; *ARGV*
                                        ;=>()

  ;;
  ;; Testing that eval sets aa in root scope, and that it is found in nested scope
  ;; (t (let* (b 12) (do (eval (read-string "(def! aa 7)")) aa ))
  ;;    7)

                                        ;>>> soft=True
                                        ;>>> optional=True
  ;;
  ;; -------- Optional Functionality --------

  ;; Testing comments in a file
  ;; (t (load-file "../tests/incB.mal")
  ;;    nil)
  ;; (t (inc4 7)
  ;;    11)
  ;; (t (inc5 7)
  ;;    12)

  ;; Testing map literal across multiple lines in a file
  ;; (t (load-file "../tests/incC.mal")
  ;;    nil)
  ;; mymap
                                        ;=>{"a" 1}

  ;; Checking that eval does not use local environments.
  ;; (def! a 1)
                                        ;=>1
  ;; (t (let* (a 2) (eval (read-string "a")))
  ;;    1)

  ;; Non alphanumeric characters in comments in read-string
  ;; (t (read-string "1;!")
  ;;    1)
  ;; (t (read-string "1;\"")
  ;;    1)
  ;; (t (read-string "1;#")
  ;;    1)
  ;; (t (read-string "1;$")
  ;;    1)
  ;; (t (read-string "1;%")
  ;;    1)
  ;; (t (read-string "1;'")
  ;;    1)
  ;; (t (read-string "1;\\")
  ;;    1)
  ;; (t (read-string "1;\\\\")
  ;;    1)
  ;; (t (read-string "1;\\\\\\")
  ;;    1)
  ;; (t (read-string "1;`")
  ;;    1)
  ;; ;;; Hopefully less problematic characters can be checked together
  ;; (t (read-string "1; &()*+,-./:;<=>?@[]^_{|}~")
  ;;    1)
  )
