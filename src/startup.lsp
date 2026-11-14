
(emit "Startup\n")

(def defn (macro (_name _args _body)
  (list 'def _name (list 'fn _args _body))))

;; Is this correct??
(def defmacro (macro (_name _args _body)
  (list 'def _name (list 'macro _args _body))))

(defn not (expr) (if expr nil else #t))

;; Set up the screen.

(def ESC "\x1b")

(defn fg-green  (a)
  (emit ESC "[92m"))

(defn fg-bold  (a)
  (emit ESC "[1m"))

(fg-green 99)
(fg-bold 99)

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



