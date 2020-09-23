(in-ns 'test.run)

(require '[test-compile.test2-eval :as test2])
(require '[test-compile.test3-env :as test3])
(require '[test-compile.test4 :as test4])
(require '[test-compile.test6-file :as test6])
(require '[test-compile.test7 :as test7])
(require '[test-compile.test8 :as test8])


;; ;; (require '[test-compile.test5-tco :as test6])
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

  ;; ;; Testing variable length arguments

  (t ( (fn [& more] (count more)) 1 2 3)
     3)
  ;; (t ( (fn [& more] (list? more)) 1 2 3)
  ;;    true)
  (t ( (fn [& more] (count more)) 1)
     1)
  (t ( (fn [& more] (count more)) )
     0)
  ;; ;; (t ( (fn [& more] (list? more)) )
  ;; ;;    true)
  (t ( (fn [a & more] (count more)) 1 2 3)
     2)
  (t ( (fn [a & more] (count more)) 1)
     0)
  ;; (t ( (fn [a & more] (list? more)) 1)
  ;;    true)


  ;; ;; Testing language defined not function
  (t (not false)
     true)
  (t (not nil)
     true)
  (t (not true)
     false)
  (t (not "a")
     false)
  (t (not 0)
     false))

(defn run-tests []
  (pr "-------------- Running tests ---------------------")

  (test2/test)
  (test3/test)

  (pr "test4")
  (test4/test4a)
  (test4/test4b)
  (test4/test4c) ;;TODO
  (test4/test4d)
  (test4/test4e)

  (pr "test6")
  (test6/test)
  (pr "test7")
  (test7/test7a)

  (test7/test7c)
  (pr "test8")
  (test8/test)
  (pr "-------------- Done running tests ---------------------")

  ;; (test5/test)
  ;; (test7/test7b)
  )
