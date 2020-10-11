(in-ns 'test.test7)


(def a (list 2 3))

(def a2 (list 1 2))
(def b (list 3 4))
(def a3 8)

(def b2 (quote (1 "b" "d")))
(def c (quote (1 "b" "d")))
(def a4 8)
(def b3 '(1 "b" "d"))

(def c2 '(1 "b" "d"))
(def a5 8)
(def c3 '(1 "b" "d"))

(defn test7a []
  ;; Testing cons function
  (t (cons 1 (list))
     (1))
  (t (cons 1 (list 2))
     (1 2))
  (t (cons 1 (list 2 3))
     (1 2 3))
  (t (cons (list 1) (list 2 3))
     ((1) 2 3))

  (t (cons 1 a)
     (1 2 3))
  (t a
     (2 3))

  ;; Testing concat function
  
  (t (concat (list 1 2) (list 3 4))
     (1 2 3 4))
  (t (concat (list) (list))
     ())
  
  ;; (t (concat)
  ;;    ())
  ;; (t (concat (list 1 2))
  ;;    (1 2))
  ;; (t (concat (list 1 2) (list 3 4) (list 5 6))
  ;;    (1 2 3 4 5 6))
  ;; (t (concat (concat))
  ;;    ())

  ;; (t (concat a b (list 5 6))
  ;;    (1 2 3 4 5 6))
  (t a2
     (1 2))
  (t b
     (3 4))

  ;; Testing regular quote
  (t (quote 7)
     7)
  (t (quote (1 2 3))
     (1 2 3))
  (t (quote (1 2 (3 4)))
     (1 2 (3 4)))
)

;; (defn test7b []
;;   ;; Testing simple quasiquote
;;   (t (quasiquote 7)
;;      7)
;;   (t (quasiquote (1 2 3))
;;      (1 2 3))
;;   (t (quasiquote (1 2 (3 4)))
;;      (1 2 (3 4)))
;;   (t (quasiquote (nil))
;;      (nil))

;;   ;; Testing unquote
;;   (t (quasiquote (unquote 7))
;;      7)
;;                                         ;=>8
;;   (t (quasiquote a3)
;;      a3)
;;   (t (quasiquote (unquote a3))
;;      8)
;;   (t (quasiquote (1 a3 3))
;;      (1 a3 3))
;;   (t (quasiquote (1 (unquote a3) 3))
;;      (1 8 3))
;;                                         ;=>(1 "b" "d")
;;   (t (quasiquote (1 b2 3))
;;      (1 b2 3))
;;   (t (quasiquote (1 (unquote b2) 3))
;;      (1 (1 "b" "d") 3))
;;   (t (quasiquote ((unquote 1) (unquote 2)))
;;      (1 2))

;;   ;; Testing splice-unquote
;;                                         ;=>(1 "b" "d")
;;   (t (quasiquote (1 c 3))
;;      (1 c 3))
;;   (t (quasiquote (1 (splice-unquote c) 3))
;;      (1 1 "b" "d" 3)))

(defn test7c []
  ;; Testing symbol equality
  (t (= (quote abc) (quote abc)) true)
  (t (= (quote abc) (quote abcd)) false)
  (t (= (quote abc) "abc") false)
  (t (= "abc" (quote abc)) false)
  ;; (t (= "abc" (str (quote abc))) true)
  (t (= (quote abc) nil) false)
  (t (= nil (quote abc)) false)

;;;;; Test quine
;;; TODO: needs expect line length fix
;;;((fn* [q] (quasiquote ((unquote q) (quote (unquote q))))) (quote (fn* [q] (quasiquote ((unquote q) (quote (unquote q)))))))
;;;=>((fn* [q] (quasiquote ((unquote q) (quote (unquote q))))) (quote (fn* [q] (quasiquote ((unquote q) (quote (unquote q)))))))

                                        ;>>> deferrable=True
  ;;
  ;; -------- Deferrable Functionality --------

  ;; Testing ' (quote) reader macro
  (t '7
     7)
  (t '(1 2 3)
     (1 2 3))
  (t '(1 2 (3 4))
     (1 2 (3 4)))

  ;; Testing cons and concat with vectors

  (t (cons [1] [2 3])
     ([1] 2 3))
  (t (cons 1 [2 3])
     (1 2 3))
  ;; (t (concat [1 2] (list 3 4) [5 6])
  ;;    (1 2 3 4 5 6))
  ;; (t (concat [1 2])
  ;;    (1 2))


                                        ;>>> optional=True
  ;;
  ;; -------- Optional Functionality --------

  ;; Testing ` (quasiquote) reader macro
  ;; (t `7 7)
                                        ;=>7
  ;; (t `(1 2 3)
  ;;    (1 2 3))
  ;; (t `(1 2 (3 4))
  ;;    (1 2 (3 4)))
  ;; (t `(nil)
  ;;    (nil))

  ;; Testing ~ (unquote) reader macro
  ;; (t `~7 7)
                                        ;=>7
                                        ;=>8
  ;; (t `(1 ~a4 3)
  ;;    (1 8 3))
                                        ;=>(1 "b" "d")
  ;; (t `(1 b3 3)
  ;;    (1 b3 3))
  ;; (t `(1 ~b3 3)
  ;;    (1 (1 "b" "d") 3))

  ;; Testing ~@ (splice-unquote) reader macro
                                        ;=>(1 "b" "d")
  ;; (t `(1 c2 3)
  ;;    (1 c2 3))
  ;; (t `(1 ~@c2 3)
  ;;    (1 1 "b" "d" 3))

  ;; Testing unquote with vectors
                                        ;=>8
  ;; (t `[1 a5 3]
  ;;    (1 a5 3))
;;; TODO: fix this
;;;;=>[1 a 3]

  ;; Testing splice-unquote with vectors
                                        ;=>(1 "b" "d")
  ;; (t `[1 ~@c3 3]
  ;;    [1 1 "b" "d" 3])
  
  )


