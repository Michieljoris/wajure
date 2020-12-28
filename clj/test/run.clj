(in-ns 'test.run)

(require '[test.test2-eval :as test2])
(require '[test.test-partial :refer [test-partial]])
(require '[test.test3-env :as test3])
(require '[test.test4 :as test4])
(require '[test.test6-file :as test6])
(require '[test.test7 :as test7])
(require '[test.test8 :as test8])

;; ;; (require '[test-compile.test5-tco :as test6])



(defn run-tests []
  (pr "-------------- Running tests ---------------------")

  (test2/test)
  (test-partial)

  (test3/test)

  (pr "test4")
  (test4/test4a)
  (pr "test4b")
  (test4/test4b)
  (pr "test4c")
  (test4/test4c)
  (pr "test4d")
  (test4/test4d)
  (pr "test4e")
  (test4/test4e)

  (pr "test6")
  (test6/test)
  (pr "test7")
  (test7/test7a)

  (test7/test7c)
  (pr "test8")
  (test8/test)
  (test8/test-self-recursive-fn)
  ;; (pr "-------------- Done running tests ---------------------")

  ;; (test5/test)
  ;; (test7/test7b)
  )
