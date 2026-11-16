
(emit "Startup\n\n\n")

(def defn (macro (_name _args _body)
  (list 'def _name (list 'fn _args _body))))

;; Is this correct??
(def defmacro (macro (_name _args _body)
  (list 'def _name (list 'macro _args _body))))

(defn not (expr) (if expr nil else #t))

;; Set up the screen.

(def ESC "\e")

(defn set-color (color)
  (emit ESC "[" color "m"))

(defn test-color (color)
  (do
    (set-color color)
    (emit color)
    (emit "  ")))

;; (test-color "90")
;; (test-color "91")
;; (test-color "92")
;; (test-color "93")
;; (test-color "94")
 (test-color "95")
 (test-color "96")
 (test-color "97")
;; 
 (test-color "100")
 (test-color "101")
 (test-color "102")
;; (test-color "103")
;; (test-color "104")
;; (test-color "105")
;; (test-color "106")
;; (test-color "107")
 
(defn fg-bold  (a)
  (emit ESC "[1m"))

(defn screen-home (a)
  (emit ESC "[H"))

(defn screen-clear-eos (a)
  (emit ESC "[J"))

(set-color "97")
(set-color "100")
(screen-home)
(screen-clear-eos)

;; List utils

(defn ffirst (lst) (first (first lst)))

(defn rrest (lst) (rest (rest lst)))


;; Value types

(def LISP_NIL 0)
(def LISP_ERROR 1)
(def LISP_CONS 2)
(def LISP_INTEGER 3)
(def LISP_SYMBOL 4)
(def LISP_STRING 5)
(def LISP_PRIMITIVE 6)
(def LISP_FUNCTION 7)
(def LISP_MACRO 8)
(def LISP_ENV 9)


(defn cons? (x) (= (type x) LISP_CONS))
(defn int? (x) (= (type x) LISP_INTEGER))
(defn symbol? (x) (= (type x) LISP_SYMBOL))
(defn macro? (x) (= (type x) LISP_MACRO))
(defn function? (x) (= (type x) LISP_FUNCTION))



