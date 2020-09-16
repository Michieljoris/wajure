(in-ns 'foo-fox.bar-bax)


(defn test1a []
  (t 1 1)
  (print "from f1"))

(defn test1b [] (print "from f2")
 (t 1 2)
  )
