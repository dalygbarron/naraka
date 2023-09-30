(defn hitler []
    "yeah"
    (prompt :a
        (var i 0)
        (while (< i 100000)
            (print "h" i)
            (++ i)
            (if (= i 5) (return :a 69))
            (yield))))

(defn gaming []
    "yee"
    (var i 0)
    (while (< i 100000)
        (print i)
        (++ i)))

(fn []
    (print (hitler))
    (gaming))