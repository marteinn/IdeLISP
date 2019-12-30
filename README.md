# IdeLISP
This is a LISP i'm putting together while reading [Build Your Own Lisp](http://www.buildyourownlisp.com/). Inspired by Lispy, Common Lisp and Hy.


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
(let '(combine) (fn '(x y) '(+ x y)))
(combine 2 1)
(== 1 1)
(if (== 1 1)
    '(print 1)
    '(print 2)
)
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


## TODO
- [x] Add builtin for defining function, ex `defun`
- [x] Add builtin for `print`
- [ ] Add string type


## The name?
Its a pun on the french name for dogmatix in Asterix/Obelix, which is Idéfix.
