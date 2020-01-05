## General

## Example syntax

```
+ 1 1
>> 2
```

```
+ (* 5 5) (+ 2 3)
>> 30
```

## Types

### Number

Integer represented in signed 64 bit integer.

```
1
```

(Max value is `−9,223,372,036,854,775,807`, `+9,223,372,036,854,775,807`)

### Decimal

A 64 bit floating point in the [IEEE 754 double precision floating point format](https://en.wikipedia.org/wiki/Double-precision_floating-point_format).

```
1.1
```

### Function

```
(fn '(arg1 arg2) '(+ arg1 arg2))
```

### Symbol

Pointer to a value, used in quoted expressions.

```
'(symbol1 symbol2)
```

### Keyword

Symbolic identifiers that evaluates to themselves

```
:symbol
>> :symbol
```

### Builtin

Functions that are built into the evaluator

```
(join 1 2 3)

(type join)
>> "Builtin"
```

### String

```
"hello"

(type "hello")
>> "String"
```

### S-Expressions

Represents tree data structure.

```
(fn (1 2 3))
```

### Q-Expressions

Represents unevaluated (`quoted`) expressions

```
'(fn (1 2 3))'
```

### HashMap

```
(hashmap '(:key 1))
```

## Globals

### `def`

Set global variables

```
(def '(x) 1)
x
>> 1

(def '(x y z) 1 2 3)
z
>> 3
```

### `defl`

Set local variables

```
(defl '(x) 1)
x
>> 1
```

### `let`

Bind variables and evaluate expressions in a local context

```
(let '(firstname lastname) '("tom" "waits")
    '(concat firstname " " lastname))
```

## Conditionals

### `If`

```
(if (== 1 1) '(+ 1 1) '(+ 2 2))
>> 4
```

### `cond`

Evaluates pairs with test and value, if test is true the value are returned. From standard library.

```
(cond
    '((== x 0) "zero")
    '((== x 1) "one")
    '(true "other"))
```

## Lists

### `list`
### `head`
### `tail`
### `join`
### `eval`

### `any`

Returns true if any of the elements match predicate. From the standard library.

```
(any not '(0 0))
```

### `all`

Returns true if all elements match predicate. From the standard library.

```
(all not '(0 0))
```

### `empty?`

Returns true if list is empty

```
(empty? '())
```

## Functions

### `defn`

Creates function and adds it to the global scope

```
(defn :plus-two '(x) '(+ 2 x))
(plus-two 2)
>> 4
```

### `fn`

Creates a lambda function

```
(defl '(plus-two) (fn '(x) '(+ 2 x)))
(plus-two 2)
>> 4
```

#### Variadic function

```
(defn '(show-all) '(&rest rest) '(print rest))
(show-all 1 2 3)
>> '(1 2 3)
```

## Strings

### `str`

Cast value to a string

```
(str 1)
>> "1"
(str "hi")
>> "hi"
(str list)
>> ""
```

### `concat`

```
(concat "hello " "John" " " "McCarthy")
>> "hello John McCarthy"
```

### `str-split`

Transforms string into a list of chars

```
(str-split "scott walker")
>> '("s" "c" "o" "t" "t" " " "w" "a" "l" "k" "e" "r")
```

### `str-join`

Convert a list of values to a string. (From the standard library).

```
(str-join '("s" "c" "o" "t" "t")'
>> "scott"
```

### `lower-case`

Convert string to lowercase

```
(lower-case "HELLO WORLD")
>> "hello world"
```

### `upper-case`

Convert string to uppercase

```
(upper-case "hello world")
>> "HELLO WORLD"
```

## Macros

```
`(list 1 2 3)
>>> {1 2 3}
```

## Operator table

| Operator  | Binary                         | Unary                           |
| --------- | ------------------------------ | ------------------------------- |
| `+`       | Addition                       |                                 |
| `/`       | Division                       |                                 |
| `-`       | Subtraction                    | Arithmetic negation             |
| `%`       | Modulo                         |                                 |
| `^`       | Exponent                       |                                 |
| `min`     | Min value                      |                                 |
| `max`     | Max value                      |                                 |
| `==`      | Equality                       |                                 |
| `!=`      | Inequality                     |                                 |
| `<`       | Less than                      |                                 |
| `>`       | Greater than                   |                                 |
| `<=`      | Less or eqal than              |                                 |
| `>=`      | Greater or eqal than           |                                 |
| `and`     | Logical and                    |                                 |
| `or`      | Logical or                     |                                 |
| `not`     |                                | Logical negation                |

These operators are called like this

```
(+ 1 1 1)
(/ 10 2)
(- 5 2)
(% 5 2)
(^ 10 3)
(min 1 100)
(max 200 1000)
(== 1 1)
(!= 1 0)
(< 1 10)
(<= 10 10)
(> 2 1)
(>= 2 2)
(and (== 1 1) 1 (> 3 2))
(or (== 1 0) (== 2 2))
(not 0)
```

## System

### `load`

Loads and evaluates external code.

```
(load "standard.ilisp")
>> ()
```

### `exit`

Exits the prompt.

```
exit ()
```

### `print`
### `type`
