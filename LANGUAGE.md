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

- Number
- Decimal
- Symbol
- Function
- String

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

### `exit`

Exits the prompt.

```
exit ()
```

## If

```
(if (== 1 1) '(+ 1 1) '(+ 2 2))
>> 4
```

## Lists

### `head`
### `tail`
### `join`
### `list`

## Functions

### `defn`

Creates function and adds it to the global scope

```
(defn '(plus-two) '(x) '(+ 2 x))
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
| `^        | Exponent                       |                                 |
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

## Builtins

| Signature                                                                    |
| ---------------------------------------------------------------------------- |
| `eval`                                                                       |
| `head`                                                                       |
| `tail`                                                                       |
| `join`                                                                       |
| `list`                                                                       |
| `join`                                                                       |

## External modules

```
(load "standard.ilisp")
```
