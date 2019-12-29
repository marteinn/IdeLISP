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

### `fn`

Function constructor

```
(let '(plus-two) (fn '(x) '(+ 2 x)))
(plus-two 2)
>> 4
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
