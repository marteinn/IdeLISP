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

### `set-v`

```
(set-v `(x) 1)
x
>> 1

(set-v `(x y z) 1 2 3)
z
>> 3
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
