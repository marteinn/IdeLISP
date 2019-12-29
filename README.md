# IdeLISP
This is a LISP i'm putting together while reading [Build Your Own Lisp](http://www.buildyourownlisp.com/). Inspired by Lispy, Common Lisp and Hy.

## Getting started
- `make build`
- `./bin/idelisp`

## Example syntax

```
(def '(x) 1)
(+ x 1)
(let '(combine) (fn '(x y) '(+ x y)))
(combine 2 1)
(let '(plus-two) (fn '(x) '(+ 2 x)))
(plus-two 2)
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
- Add builtin for defining function, ex `defun`
- Add builtin for `print`

## The name?
Its a pun on the french name for dogmatix in Asterix/Obelix, which is Idéfix.
