![IdeLISP](https://raw.githubusercontent.com/marteinn/IdeLISP/master/img/idelisp-logo.png)

# IdeLISP
This is a LISP I put together while reading [Build Your Own Lisp](http://www.buildyourownlisp.com/). Inspired by Lispy, Clojure and Common Lisp.


## Installing
- Clone [MPC](https://github.com/orangeduck/mpc) and drop `mpc.c` and `mpc.h` in the project root
- `make build`

_(Please note this project is currently not cross-platform, and probably never will be. Will much likely work when compiling on Mac OS)_


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

### Compiling and running as WebAssembly

- `make build_wasm`
- `cd wasm`
- Serve the directory in a webserver: `python -m SimpleHTTPServer`
- open "http://localhost:8000/idelisp.html"

## Example syntax

```
(load "standard.ilisp")
(+ 1 2)
(* (+ 2 3.1) (* 2 3))
{:my_key "value"}
(def :x 1)
(+ x 1)
(defn :plus-two '(x) '(+ 2 x))
(plus-two 2)
(defl :combine (fn '(x y) '(+ x y)))
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
(let '(a b) '(1 2)'
    '(print a))
(zero-arity-fn ())
```

## Language syntax
See the [language](https://github.com/marteinn/IdeLISP/blob/master/LANGUAGE.md) specification


## The name?
Its a pun on the french name for dogmatix in Asterix/Obelix, which is Idéfix.
