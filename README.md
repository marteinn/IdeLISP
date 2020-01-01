# IdeLISP
This is a LISP i'm putting together while reading [Build Your Own Lisp](http://www.buildyourownlisp.com/). Inspired by Lispy, Clojure and Common Lisp.


## Installing
- `make build`


## Running

### Using the REPL

```
./bin/idelisp
IdeLISP (type exit() to quit)
>> + 1 1
2
```

### Executing a file

```
./bin/idelisp -f example.ilisp
2
```


## Example syntax

```
(+ 1 2)
(* (+ 2 3) (* 2 3))
(def '(x) 1)
(+ x 1)
(defn '(plus-two) '(x) '(+ 2 x))
(plus-two 2)
(defl '(combine) (fn '(x y) '(+ x y)))
(combine 2 1)
(== 1 1)
(if (== 1 1)
    '(print 1)
    '(print 2)
)
(if (and (== 1 1) (!= 1 0))
    '(print 1)
    '(print 2)
)
(not 0)
; I am a comment
(load "curry.ilisp")
(let '(a b) '(1 2)'
    '(print a))
```


## Debugging
- Use lldb
- [Valgrind](https://valgrind.org/)


## Documentation
- [C reference](https://en.cppreference.com/w/c)
- [Polish notation](https://en.wikipedia.org/wiki/Polish_notation)
- [MPC](https://github.com/orangeduck/mpc)
- [Cons](https://en.wikipedia.org/wiki/Cons)
- [Common Lisp](https://en.wikipedia.org/wiki/Common_Lisp)


## The name?
Its a pun on the french name for dogmatix in Asterix/Obelix, which is Idéfix.
