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

### `let`

Set local variables

```
(let '(x) 1)
x
>> 1
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
(let '(plus-two) (fn '(x) '(+ 2 x)))
(plus-two 2)
>> 4
```

#### Variadic function

```
(defn '(show-all) '(&rest rest) '(print rest))
(show-all 1 2 3)
>> '(1 2 3)
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

## Builtins

| Signature                                                                    |
| ---------------------------------------------------------------------------- |
| `eval`                                                                       |
| `head`                                                                       |
| `tail`                                                                       |
| `join`                                                                       |
| `list`                                                                       |
| `join`                                                                       |
| `setv`                                                                       |
