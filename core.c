#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

enum {
    IDEOBJ_ERR,
    IDEOBJ_NUM,
    IDEOBJ_SYMBOL,
    IDEOBJ_SEXPR,
    IDEOBJ_QEXPR,
    IDEOBJ_DECIMAL,
    IDEOBJ_BUILTIN,
    IDEOBJ_FUN,
    IDEOBJ_STR,
    IDEOBJ_HASHMAP,
    IDEOBJ_KEYWORD
};


struct ideobj;
struct ideenv;
typedef struct ideobj ideobj;
typedef struct ideenv ideenv;

typedef ideobj*(*ibuiltin)(ideenv*, ideobj*);

struct ideobj {
    int type;

    long num;
    double decimal;
    char* err;
    char* symbol;
    char* keyword;
    char* str;
    ibuiltin builtin;

    ideenv* env;
    ideobj* params;
    ideobj* body;

    int count;
    struct ideobj** cell;
    struct ideobj** keys;
};

struct ideenv {
    ideenv* parent;
    int count;
    char** symbols;
    ideobj** values;
    int depth;
};

mpc_parser_t* Decimal;
mpc_parser_t* Number;
mpc_parser_t* String;
mpc_parser_t* Symbol;
mpc_parser_t* Comment;
mpc_parser_t* HashMap;
mpc_parser_t* Keyword;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* IdeLISP;


ideenv* ideenv_new(void);            // Forward declaration
ideenv* ideenv_new_enclosed(ideenv* env);       // Forward declaration


ideobj* ideobj_err(char* format, ...) {
    ideobj* obj = malloc(sizeof(ideobj));
    obj->type = IDEOBJ_ERR;

    va_list value_list;
    va_start(value_list, format);

    obj->err = malloc(512);
    vsnprintf(obj->err, 511, format, value_list);

    obj->err = realloc(obj->err, strlen(obj->err)+1);
    va_end(value_list);

    return obj;
}

ideobj* ideobj_num(long val) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_NUM;
    obj->num = val;
    return obj;
}

ideobj* ideobj_decimal(double val) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_DECIMAL;
    obj->decimal = val;
    return obj;
}

ideobj* ideobj_symbol(char* symbol) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_SYMBOL;
    obj->symbol = malloc(strlen(symbol) + 1);
    strcpy(obj->symbol, symbol);
    return obj;
}

ideobj* ideobj_sexpr(void) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_SEXPR;
    obj->count = 0;
    obj->cell = NULL;
    return obj;
}

ideobj* ideobj_qexpr(void) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_QEXPR;
    obj->count = 0;
    obj->cell = NULL;
    return obj;
}

ideobj* ideobj_builtin(ibuiltin builtin) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_BUILTIN;
    obj->builtin = builtin;
    return obj;
}

ideobj* ideobj_fun(ideobj* params, ideobj* body) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_FUN;
    obj->env = ideenv_new();
    obj->params = params;
    obj->body = body;
    return obj;
}

ideobj* ideobj_str(char* str) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_STR;
    obj->str = malloc(strlen(str) + 1);
    strcpy(obj->str, str);
    return obj;
}

ideobj* ideobj_keyword(char* keyword) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_KEYWORD;
    obj->keyword = malloc(strlen(keyword) + 1);
    strcpy(obj->keyword, keyword);
    return obj;
}

ideobj* ideobj_hashmap(void) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_HASHMAP;
    obj->count = 0;
    obj->keys = NULL;
    obj->cell = NULL;
    return obj;
}

char* idetype_name(int type) {
    switch (type) {
        case IDEOBJ_ERR: return "Error";
        case IDEOBJ_SYMBOL: return "Symbol";
        case IDEOBJ_NUM: return "Number";
        case IDEOBJ_DECIMAL: return "Decimal";
        case IDEOBJ_BUILTIN: return "Builtin";
        case IDEOBJ_FUN: return "Function";
        case IDEOBJ_QEXPR: return "Quoted Expression";
        case IDEOBJ_SEXPR: return "S-Expression";
        case IDEOBJ_STR: return "String";
        case IDEOBJ_KEYWORD: return "Keyword";
        case IDEOBJ_HASHMAP: return "HashMap";
        default: return "Unknown";
    }
}

void ideenv_del(ideenv* env);

void ideobj_del(ideobj* obj) {
    switch (obj->type) {
        case IDEOBJ_ERR: free(obj->err); break;
        case IDEOBJ_SYMBOL: free(obj->symbol); break;
        case IDEOBJ_KEYWORD: free(obj->keyword); break;
        case IDEOBJ_NUM: break;
        case IDEOBJ_DECIMAL: break;
        case IDEOBJ_BUILTIN: break;
        case IDEOBJ_FUN:
            ideenv_del(obj->env);
            ideobj_del(obj->params);
            ideobj_del(obj->body);
            break;
        case IDEOBJ_QEXPR:
        case IDEOBJ_SEXPR:
            for (int i=0; i<obj->count; i++) {
                ideobj_del(obj->cell[i]);
            }
            free(obj->cell);
            break;
        case IDEOBJ_STR: free(obj->str); break;
        case IDEOBJ_HASHMAP:
            for (int i=0; i<obj->count; i++) {
                ideobj_del(obj->keys[i]);
                ideobj_del(obj->cell[i]);
            }
            free(obj->keys);
            free(obj->cell);
            break;
    }

    free(obj);
}

ideenv* ideenv_copy(ideenv* env);
void ideenv_print(ideenv* env);

ideobj* ideobj_copy(ideobj* obj) {
    ideobj* copy = malloc(sizeof(ideobj));
    copy->type = obj->type;

    switch (obj->type) {
        case IDEOBJ_BUILTIN:
            copy->builtin = obj->builtin;
            break;
        case IDEOBJ_NUM:
            copy->num = obj->num;
            break;
        case IDEOBJ_DECIMAL:
            copy->decimal= obj->decimal;
            break;
        case IDEOBJ_ERR:
            copy->err = malloc(strlen(obj->err) + 1);
            strcpy(copy->err, obj->err);
            break;
        case IDEOBJ_SYMBOL:
            copy->symbol = malloc(strlen(obj->symbol) + 1);
            strcpy(copy->symbol, obj->symbol);
            break;
        case IDEOBJ_QEXPR:
        case IDEOBJ_SEXPR:
            copy->count = obj->count;
            copy->cell = malloc(sizeof(ideobj) * copy->count);
            for (int i=0; i<obj->count; i++) {
                copy->cell[i] = ideobj_copy(obj->cell[i]);
            }
            break;
        case IDEOBJ_FUN:
            copy->env = ideenv_copy(obj->env);
            copy->params = ideobj_copy(obj->params);
            copy->body = ideobj_copy(obj->body);
            break;
        case IDEOBJ_STR:
            copy->str = malloc(strlen(obj->str) + 1);
            strcpy(copy->str, obj->str);
            break;
        case IDEOBJ_KEYWORD:
            copy->keyword = malloc(strlen(obj->keyword) + 1);
            strcpy(copy->keyword, obj->keyword);
            break;
        case IDEOBJ_HASHMAP:
            copy->count = obj->count;
            copy->keys = malloc(sizeof(ideobj) * copy->count);
            copy->cell = malloc(sizeof(ideobj) * copy->count);
            for (int i=0; i<obj->count; i++) {
                copy->keys[i] = ideobj_copy(obj->keys[i]);
                copy->cell[i] = ideobj_copy(obj->cell[i]);
            }
            break;
    }

    return copy;
}

void ideobj_println(ideobj* obj);

int ideobj_eq(ideobj* left, ideobj* right) {
    if (left->type != right->type) {
        return 0;
    }

    switch (left->type) {
        case IDEOBJ_NUM:
            return left->num == right->num;
        case IDEOBJ_DECIMAL:
            return fabs(left->decimal - right->decimal) <= 0.00001;
        case IDEOBJ_ERR:
            return strcmp(left->err, right->err) == 0;
        case IDEOBJ_SYMBOL:
            return strcmp(left->symbol, right->symbol) == 0;
        case IDEOBJ_BUILTIN:
            return left->builtin == right->builtin;
        case IDEOBJ_FUN:
            return ideobj_eq(left->params, right->params)
                && ideobj_eq(left->body, right->body);
        case IDEOBJ_QEXPR:
        case IDEOBJ_SEXPR:
            if (left->count != right->count) {
                return 0;
            }

            for (int i=0; i<left->count; i++) {
                if (ideobj_eq(left->cell[0], right->cell[0]) == 0) {
                    return 0;
                }
            }

            return 1;
        case IDEOBJ_HASHMAP:
            if (left->count != right->count) {
                return 0;
            }
            for (int i=0; i<left->count; i++) {
                if (ideobj_eq(left->keys[0], right->keys[0]) == 0) {
                    return 0;
                }
                if (ideobj_eq(left->cell[0], right->cell[0]) == 0) {
                    return 0;
                }
            }

            return 1;

        case IDEOBJ_STR:
            return strcmp(left->str, right->str) == 0;
        case IDEOBJ_KEYWORD:
            return strcmp(left->keyword, right->keyword) == 0;
    }

    return 0;
}

int ideobj_truthy(ideobj* obj) {
    switch (obj->type) {
        case IDEOBJ_NUM:
            return obj->num == 1;
        case IDEOBJ_DECIMAL:
            return fabs(obj->decimal - 0.00001) <= 0.00001;
        case IDEOBJ_ERR:
        case IDEOBJ_SYMBOL:
        case IDEOBJ_BUILTIN:
        case IDEOBJ_FUN:
            return 1;
        case IDEOBJ_QEXPR:
        case IDEOBJ_SEXPR:
            return obj->count > 0;
        case IDEOBJ_STR:
            return strlen(obj->str) > 0;
        case IDEOBJ_KEYWORD:
            return strlen(obj->str) > 0;
    }

    return 0;
}

void ideobj_print(ideobj* obj);

void ideobj_expr_print(ideobj* obj, char* open, char* close) {
    printf("%s", open);

    for (int i=0; i<obj->count; i++) {
        if (i > 0) {
            putchar(' ');
        }
        ideobj_print(obj->cell[i]);
    }

    printf("%s", close);
}

void lval_print_str(ideobj* obj) {
    char* escaped = malloc(strlen(obj->str)+1);
    strcpy(escaped, obj->str);

    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);

    free(escaped);
}

void ideobj_print(ideobj* obj) {
    switch (obj->type) {
        case IDEOBJ_ERR:
            printf("Error: %s", obj->err);
            break;
        case IDEOBJ_NUM:
            printf("%li", obj->num);
            break;
        case IDEOBJ_DECIMAL:
            printf("%.10g", obj->decimal);
            break;
        case IDEOBJ_SYMBOL:
            printf("%s", obj->symbol);
            break;
        case IDEOBJ_KEYWORD:
            printf(":%s", obj->keyword);
            break;
        case IDEOBJ_SEXPR:
            ideobj_expr_print(obj, "(", ")");
            break;
        case IDEOBJ_QEXPR:
            ideobj_expr_print(obj, "'(", ")");
            break;
        case IDEOBJ_BUILTIN:
            printf("<builtin>");
            break;
        case IDEOBJ_FUN:
            printf("(fn ");
            ideobj_print(obj->params);
            putchar(' ');
            ideobj_print(obj->body);
            putchar(')');
            break;
        case IDEOBJ_STR:
            lval_print_str(obj);
            break;
        case IDEOBJ_HASHMAP:
            putchar('{');
            for (int i=0; i<obj->count; i++) {
                if (i > 0) {
                    putchar(' ');
                }
                ideobj_print(obj->keys[i]);
                printf(": ");
                ideobj_print(obj->cell[i]);
            }
            putchar('}');
            break;
    }
}

void ideobj_println(ideobj* obj) {
    ideobj_print(obj);
    putchar('\n');
}

ideenv* ideenv_new(void) {
    ideenv* env = malloc(sizeof(ideenv));
    env->parent = NULL;
    env->count = 0;
    env->symbols = NULL;
    env->values = NULL;
    env->depth = -1;
    return env;
}

ideenv* ideenv_new_enclosed(ideenv* env) {
    ideenv* enclosed_env = ideenv_new();
    enclosed_env->parent = env;
    enclosed_env->depth = env->depth+1;
    return enclosed_env;
}

void ideenv_del(ideenv* env) {
    for (int i=0; i<env->count; i++) {
        free(env->symbols[i]);
        ideobj_del(env->values[i]);
    }

    free(env->symbols);
    free(env->values);
    free(env);
}

void ideenv_print(ideenv* env) {
    putchar('(');
    for (int i=0; i<env->count; i++) {
        if (i > 0) {
            printf(", ");
        }
        printf("%s: ", env->symbols[i]);
        ideobj_print(env->values[i]);
    }

    if (env->count == 0) {
        printf("[[empty]]");
    }

    if (env->parent) {
        printf(", parent: ");
        ideenv_print(env->parent);
    }
    putchar(')');
}

void ideenv_println(ideenv* env) {
    ideenv_print(env);
    putchar('\n');
}

ideenv* ideenv_copy(ideenv* env) {
    ideenv* copy = malloc(sizeof(ideenv));
    copy->parent = env->parent;
    copy->count = env->count;
    copy->depth = env->depth;
    copy->symbols = malloc(sizeof(char*) * copy->count);
    copy->values = malloc(sizeof(ideobj*) * copy->count);

    for (int i=0; i<copy->count; i++) {
        copy->symbols[i] = malloc(strlen(env->symbols[i]) + 1);
        strcpy(copy->symbols[i], env->symbols[i]);
        copy->values[i] = ideobj_copy(env->values[i]);
    }

    return copy;
}

ideobj* ideenv_get(ideenv* env, ideobj* key) {
    for (int i=0; i<env->count; i++) {
        if (strcmp(env->symbols[i], key->symbol) == 0) {
            return ideobj_copy(env->values[i]);
        }
    }

    if (env->parent) {
        return ideenv_get(env->parent, key);
    } else {
        return ideobj_err("Unbound symbol '%s'", key->symbol);
    }
}

void ideenv_put(ideenv* env, ideobj* key, ideobj* val) {
    for (int i=0; i<env->count; i++) {
        if (
            key->type == IDEOBJ_SYMBOL &&
            strcmp(env->symbols[i], key->symbol) == 0
        ) {
            ideobj_del(env->values[i]);
            env->values[i] = ideobj_copy(val);
            return;
        }

        if (
            key->type == IDEOBJ_KEYWORD &&
            strcmp(env->symbols[i], key->keyword) == 0
        ) {
            ideobj_del(env->values[i]);
            env->values[i] = ideobj_copy(val);
            return;
        }
    }

    char *key_str;

    if (key->type == IDEOBJ_KEYWORD) {
        key_str = malloc(strlen(key->keyword) + 1);
        strcpy(key_str, key->keyword);
    }
    if (key->type == IDEOBJ_SYMBOL) {
        key_str = malloc(strlen(key->symbol) + 1);
        strcpy(key_str, key->symbol);
    }

    env->count++;
    env->values = realloc(env->values, sizeof(ideobj*) * env->count);
    env->symbols = realloc(env->symbols, sizeof(char*) * env->count);

    env->values[env->count - 1] = ideobj_copy(val);
    env->symbols[env->count - 1] = malloc(strlen(key_str) + 1);
    strcpy(env->symbols[env->count - 1], key_str);
}

void ideenv_global_put(ideenv* env, ideobj* key, ideobj* val) {
    while(env->parent) {
        env = env->parent;
    }

    ideenv_put(env, key, val);
}

ideobj* eval_tenary_number_op(ideobj* left, char* operator, ideobj* right) {
    if (strcmp(operator, "+") == 0) {
        return ideobj_num(left->num + right->num);
    }
    if (strcmp(operator, "-") == 0) {
        return ideobj_num(left->num - right->num);
    }
    if (strcmp(operator, "*") == 0) {
        return ideobj_num(left->num * right->num);
    }
    if (strcmp(operator, "/") == 0) {
        if (right->num == 0) {
            return ideobj_err("Division by zero");
        }
        return ideobj_num(left->num / right->num);
    }
    if (strcmp(operator, "%") == 0) {
        if (right->num == 0) {
            return ideobj_err("Modulo by zero");
        }
        return ideobj_num(left->num % right->num);
    }
    if (strcmp(operator, "^") == 0) {
        return ideobj_num(pow(left->num, right->num));
    }
    if (strcmp(operator, "min") == 0) {
        return ideobj_num(
            left->num < right->num ? left->num : right->num
        );
    }
    if (strcmp(operator, "max") == 0) {
        return ideobj_num(left->num > right->num ? left->num : right->num);
    }

    return ideobj_err("Invalid operator '%s'", operator);
}

int ideobj_is_numeric(ideobj *obj) {
    return obj->type == IDEOBJ_NUM || obj->type == IDEOBJ_DECIMAL;
}

ideobj* eval_tenary_decimal_op(ideobj* left, char* operator, ideobj* right) {
    double left_value = left->decimal;
    double right_value = right->decimal;

    if (left->type == IDEOBJ_NUM) {
        left_value = (double) left->num;
    }

    if (right->type == IDEOBJ_NUM) {
        right_value = (double) right->num;
    }

    if (strcmp(operator, "+") == 0) {
        return ideobj_decimal(left_value + right_value);
    }
    if (strcmp(operator, "-") == 0) {
        return ideobj_decimal(left_value - right_value);
    }
    if (strcmp(operator, "*") == 0) {
        return ideobj_decimal(left_value * right_value);
    }
    if (strcmp(operator, "/") == 0) {
        if (right->num == 0) {
            return ideobj_err("Division by zero");
        }
        return ideobj_decimal(left_value / right_value);
    }
    if (strcmp(operator, "^") == 0) {
        return ideobj_decimal(pow(left_value, right_value));
    }
    if (strcmp(operator, "min") == 0) {
        return ideobj_decimal(
            left_value < right_value ? left_value : right_value
        );
    }
    if (strcmp(operator, "max") == 0) {
        return ideobj_decimal(left_value > right_value ? left_value : right_value);
    }

    return ideobj_err("Invalid operator '%s'", operator);
}

ideobj* ideobj_list_add(ideobj* left, ideobj* right) {
    left->count++;
    left->cell = realloc(left->cell, sizeof(ideobj*) * left->count);
    left->cell[left->count-1] = right;
    return left;
}

ideobj* ideobj_pop(ideobj* obj, int i) {
    ideobj* el = obj->cell[i];

    memmove(
        &obj->cell[i], &obj->cell[i+1], sizeof(ideobj*) * (obj->count-i-1)
    );

    obj->count--;
    obj->cell = realloc(obj->cell, sizeof(ideobj*) * obj->count);
    return el;
}

ideobj* ideobj_take(ideobj* obj, int i) {
    ideobj* el = ideobj_pop(obj, i);
    ideobj_del(obj);
    return el;
}

#define IASSERT(args, cond, format, ...) \
    if (!(cond)) { \
        ideobj* error = ideobj_err(format, ##__VA_ARGS__); \
        ideobj_del(args); \
        return error; \
    }

#define IASSERT_TYPE(func, args, index, expect) \
  IASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. " \
    "Got %s, Expected %s.", \
    func, index, idetype_name(args->cell[index]->type), idetype_name(expect))

#define IASSERT_NUM(func, args, num) \
  IASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. " \
    "Got %i, Expected %i.", \
    func, args->count, num)

#define IASSERT_NOT_EMPTY(func, args, index) \
  IASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed '() for argument %i.", func, index);

ideobj* ideobj_eval(ideenv* env, ideobj* obj);

ideobj* builtin_head(ideenv* env, ideobj* obj) {
    IASSERT(
        obj,
        obj->count == 1,
        "Function 'head' received wrong number of arguments, requires 1"
        "got %i, expected %i", obj->count, 1
    );
    IASSERT(
        obj,
        obj->cell[0]->type == IDEOBJ_QEXPR,
        "Function 'head' received wrong type, got %s, expected %s",
        idetype_name(obj->cell[0]->type),
        idetype_name(IDEOBJ_QEXPR)
    );
    IASSERT(
        obj, obj->cell[0]->count != 0, "Function 'head' received empty list"
    );

    ideobj* first = ideobj_take(obj, 0);
    while(first->count > 1) {
        ideobj_del(
            ideobj_pop(first, 1)
        );
    }
    return first;
}

ideobj* builtin_tail(ideenv* env, ideobj* obj) {
    IASSERT_NUM("tail", obj, 1);
    IASSERT_TYPE("tail", obj, 0, IDEOBJ_QEXPR);
    IASSERT_NOT_EMPTY("tail", obj, 0);

    ideobj* first = ideobj_take(obj, 0);
    ideobj_del(ideobj_pop(first, 0));
    return first;
}

ideobj* builtin_list(ideenv* env, ideobj* obj) {
    obj->type = IDEOBJ_QEXPR;
    return obj;
}

ideobj* builtin_str_split(ideenv* env, ideobj *obj) {
    ideobj *list = ideobj_qexpr();
    char* source = obj->cell[0]->str;

    list->cell = malloc(sizeof(ideobj) * strlen(source));
    list->count = strlen(source);

    for (int i=0; i<strlen(source); i++) {
        char *str_char = malloc(2);
        strncpy(str_char, &source[i], 1);

        list->cell[i] = ideobj_str(str_char);
        free(str_char);
    }

    ideobj_del(obj);
    return list;
}

ideobj* builtin_str_uppercase(ideenv* env, ideobj *obj) {
    IASSERT_NUM("upper-case", obj, 1);
    IASSERT_TYPE("upper-case", obj, 0, IDEOBJ_STR);

    char* ucase_str = malloc(strlen(obj->cell[0]->str) + 1);
    strcpy(ucase_str, obj->cell[0]->str);

    for (int i=0; i<strlen(ucase_str); i++) {
        ucase_str[i] = toupper(ucase_str[i]);
    }

    ideobj_del(obj);
    return ideobj_str(ucase_str);
}

ideobj* builtin_str_lowercase(ideenv* env, ideobj *obj) {
    IASSERT_NUM("lower-case", obj, 1);
    IASSERT_TYPE("lower-case", obj, 0, IDEOBJ_STR);

    char* ucase_str = malloc(strlen(obj->cell[0]->str) + 1);
    strcpy(ucase_str, obj->cell[0]->str);

    for (int i=0; i<strlen(ucase_str); i++) {
        ucase_str[i] = tolower(ucase_str[i]);
    }

    ideobj_del(obj);
    return ideobj_str(ucase_str);
}

ideobj* ideobj_hashmap_add(ideobj* hm, ideobj* key, ideobj* val) {
    hm->count++;

    hm->keys = realloc(hm->keys, sizeof(ideobj*) * hm->count);
    hm->keys[hm->count-1] = key;

    hm->cell = realloc(hm->cell, sizeof(ideobj*) * hm->count);
    hm->cell[hm->count-1] = val;

    return hm;
}

ideobj* builtin_hashmap(ideenv* env, ideobj* obj) {
    IASSERT_NUM("hash-map", obj, 1);
    IASSERT_TYPE("hash-map", obj, 0, IDEOBJ_QEXPR);

    if ((obj->cell[0]->count % 2) != 0) {
        ideobj_del(obj);
        return ideobj_err("Uneven amount of key vs values");
    }

    ideobj *hm = ideobj_hashmap();

    while (obj->cell[0]->count) {
        ideobj *key = ideobj_pop(obj->cell[0], 0);
        ideobj *value = ideobj_pop(obj->cell[0], 0);

        ideobj_hashmap_add(hm, key, value);

        // hm->count++;

        // hm->keys = realloc(hm->keys, sizeof(ideobj*) * hm->count);
        // hm->keys[hm->count-1] = key;

        // hm->cell = realloc(hm->cell, sizeof(ideobj*) * hm->count);
        // hm->cell[hm->count-1] = value;
    }

    ideobj_del(obj);
    return hm;
}


ideobj* builtin_hashmap_key(ideenv* env, ideobj* obj) {
    IASSERT_NUM("key", obj, 2);
    IASSERT_TYPE("key", obj, 0, IDEOBJ_HASHMAP);

    ideobj *hm = ideobj_pop(obj, 0);
    ideobj *key = ideobj_pop(obj, 0);

     for (int i=0; i<hm->count; i++) {
         if (ideobj_eq(key, hm->keys[i])) {
             ideobj_del(obj);
             return hm->cell[i];
         }
     }

    return ideobj_err("Key not found");
}

ideobj* builtin_hashmap_assoc(ideenv* env, ideobj* obj) {
    IASSERT_NUM("assoc", obj, 3);
    IASSERT_TYPE("assoc", obj, 2, IDEOBJ_HASHMAP);

    ideobj *key = ideobj_pop(obj, 0);
    ideobj *val = ideobj_pop(obj, 0);
    ideobj *hm = ideobj_pop(obj, 0);

    for (int i=0; i<hm->count; i++) {
        if (ideobj_eq(key, hm->keys[i])) {
            ideobj_del(obj);
            hm->cell[i] = ideobj_copy(val);
            return hm;
        }
    }

    ideobj_hashmap_add(hm, key, val);
    return hm;
}

ideobj* ideobj_hashmap_take(ideobj *obj, int i, char* return_type) {
    ideobj* key = obj->keys[i];
    ideobj* val = obj->cell[i];

    memmove(
        &obj->keys[i], &obj->keys[i+1], sizeof(ideobj*) * (obj->count-i-1)
    );
    memmove(
        &obj->cell[i], &obj->cell[i+1], sizeof(ideobj*) * (obj->count-i-1)
    );

    obj->count--;
    obj->keys= realloc(obj->keys, sizeof(ideobj*) * obj->count);
    obj->cell = realloc(obj->cell, sizeof(ideobj*) * obj->count);

    if (strcmp(return_type, "key") == 0) {
        return key;
    }

    ideobj_del(obj);
    return val;
}

ideobj* builtin_hashmap_deassoc(ideenv* env, ideobj* obj) {
    IASSERT_NUM("dassoc", obj, 2);
    IASSERT_TYPE("dassoc", obj, 1, IDEOBJ_HASHMAP);

    ideobj *key = ideobj_pop(obj, 0);
    ideobj *hm = ideobj_pop(obj, 0);

    int index = -1;

    for (int i=0; i<hm->count; i++) {
        if (ideobj_eq(key, hm->keys[i])) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return ideobj_err("Key not found in hashmap");
    }

    memmove(
        &hm->keys[index],
        &hm->keys[index+1],
        sizeof(ideobj*) * (hm->count-index-1)
    );

    memmove(
        &hm->cell[index],
        &hm->cell[index+1],
        sizeof(ideobj*) * (hm->count-index-1)
    );

    hm->count--;
    hm->keys = realloc(hm->keys, sizeof(ideobj*) * hm->count);
    hm->cell = realloc(hm->cell, sizeof(ideobj*) * hm->count);
    return hm;

}

ideobj* builtin_str(ideenv* env, ideobj *obj) {
    IASSERT_NUM("str", obj, 1);
    char* source = "";

    switch (obj->cell[0]->type) {
        case IDEOBJ_DECIMAL: {
            int str_len = snprintf(NULL, 0, "%.10g", obj->cell[0]->decimal);
            source = malloc(str_len+1);
            snprintf(source, str_len + 1, "%.10g", obj->cell[0]->decimal);
            break;
        }
        case IDEOBJ_NUM: {
            int str_len = snprintf(NULL, 0, "%li", obj->cell[0]->num);
            source = malloc(str_len+1);
            snprintf(source, str_len + 1, "%li", obj->cell[0]->num);
            break;
        }
        case IDEOBJ_SYMBOL:
            source = malloc(strlen(obj->cell[0]->symbol) + 1);
            strcpy(source, obj->cell[0]->symbol);
            break;
        case IDEOBJ_STR:
            source = malloc(strlen(obj->cell[0]->str) + 1);
            strcpy(source, obj->cell[0]->str);
            break;
        case IDEOBJ_ERR:
            source = malloc(strlen(obj->cell[0]->err) + 1);
            strcpy(source, obj->cell[0]->err);
            break;
    }

    ideobj* str_obj = ideobj_str(source);
    ideobj_del(obj);
    return str_obj;
}

ideobj* builtin_eval(ideenv* env, ideobj* obj) {
    IASSERT(
        obj,
        obj->count == 1,
        "Function 'eval' received wrong number of arguments, requires 1"
    );
    IASSERT(
        obj,
        obj->cell[0]->type == IDEOBJ_QEXPR,
        "Function 'eval' received wrong type"
    );

    ideobj* first = ideobj_take(obj, 0);

    first->type = IDEOBJ_SEXPR;
    return ideobj_eval(env, first);
}

ideobj* ideobj_join(ideobj* left, ideobj* right) {
    for (int i = 0; i < right->count; i++) {
        left = ideobj_list_add(left, right->cell[i]);
    }

    free(right->cell);
    free(right);
    return left;
}

ideobj* builtin_concat(ideenv* env, ideobj* obj) {
    for (int i=0; i<obj->count; i++) {
        IASSERT(
            obj,
            obj->cell[i]->type == IDEOBJ_STR,
            "Function 'concat' passed incorrect type"
        );
    }

    ideobj* left = ideobj_pop(obj, 0);

    char *source = malloc(strlen(left->str) + 1);
    strcpy(source, left->str);

    ideobj_del(left);

    while(obj->count) {
        ideobj* right = ideobj_pop(obj, 0);

        char *combined = malloc(strlen(source) + strlen(right->str) + 1);
        strcpy(combined, source);
        strcat(combined, right->str);

        source = malloc(strlen(combined) + 1);
        strcpy(source, combined);

        free(combined);
        ideobj_del(right);
    }

    ideobj* concat = ideobj_str(source);

    free(source);
    ideobj_del(obj);
    return concat;
}

ideobj* builtin_error(ideenv* env, ideobj* obj) {
    IASSERT_NUM("error", obj, 1);
    IASSERT_TYPE("error", obj, 0, IDEOBJ_STR);

    ideobj* err = ideobj_err(obj->cell[0]->str);
    ideobj_del(obj);
    return err;
}

ideobj* builtin_type(ideenv* env, ideobj* obj) {
    IASSERT_NUM("type", obj, 1);

    ideobj *type = ideobj_str(
        idetype_name(obj->cell[0]->type)
    );
    ideobj_del(obj);
    return type;
}

ideobj* builtin_len(ideenv* env, ideobj* obj) {
    IASSERT_NUM("len", obj, 1);

    ideobj *len_obj;

    switch (obj->cell[0]->type) {
        case IDEOBJ_SEXPR:
        case IDEOBJ_QEXPR:
        case IDEOBJ_HASHMAP:
            len_obj = ideobj_num(obj->cell[0]->count);
            break;
        case IDEOBJ_STR:
            len_obj = ideobj_num(strlen(obj->cell[0]->str));
            break;
        default:
            len_obj = ideobj_err(
                "Type %s does not support len",
                idetype_name(obj->cell[0]->type)
            );
    }

    ideobj_del(obj);
    return len_obj;
}

ideobj* builtin_join(ideenv* env, ideobj* obj) {
    for (int i=0; i<obj->count; i++) {
        IASSERT_TYPE("join", obj, i, IDEOBJ_QEXPR);
    }

    ideobj* first = ideobj_pop(obj, 0);
    while(obj->count) {
        first = ideobj_join(first, ideobj_pop(obj, 0));
    }

    ideobj_del(obj);
    return first;
}

ideobj* builtin_op_num(ideenv* env, ideobj* obj, char* operator) {
    ideobj* acc_value = ideobj_pop(obj, 0);

    if (strcmp(operator, "-") == 0 && obj->count == 0) {
        acc_value->num = -acc_value->num;
    }

    while(obj->count > 0) {
        ideobj* right = ideobj_pop(obj, 0);
        acc_value = eval_tenary_number_op(acc_value, operator, right);
        ideobj_del(right);
    }

    ideobj_del(obj);
    return acc_value;
}

ideobj* builtin_op_decimal(ideenv* env, ideobj* obj, char* operator) {
    ideobj* acc_value = ideobj_pop(obj, 0);

    if (strcmp(operator, "-") == 0 && obj->count == 0) {
        acc_value->decimal = -acc_value->decimal;
    }

    while(obj->count > 0) {
         ideobj* right = ideobj_pop(obj, 0);
         acc_value = eval_tenary_decimal_op(acc_value, operator, right);
         ideobj_del(right);
    }

    ideobj_del(obj);
    return acc_value;
}

int ideobj_cells_is_type(ideobj* obj, int type) {
    for (int i=0; i<obj->count; i++) {
        if (obj->cell[i]->type != type) {
            return 0;
        }
    }

    return 1;
}

ideobj* builtin_op(ideenv* env, ideobj* obj, char* operator) {
    if (ideobj_cells_is_type(obj, IDEOBJ_NUM)) {
        return builtin_op_num(env, obj, operator);
    }

    if (ideobj_is_numeric(obj->cell[0])) {
        return builtin_op_decimal(env, obj, operator);
    }

    return ideobj_err("Cannot operate on non-number");
}

int is_approved_symbol(ideobj* obj) {
    return obj->type == IDEOBJ_SYMBOL || obj->type == IDEOBJ_KEYWORD;
}

ideobj* builtin_var(ideenv* env, ideobj* obj, char* func) {
    IASSERT_NUM(func, obj, 2);

    ideobj* keys = obj->cell[0];
    ideobj* values = obj->cell[1];

    if (keys->type == IDEOBJ_QEXPR) {
        IASSERT_TYPE(func, obj, 1, IDEOBJ_QEXPR);
    }

    // Single
    if (keys->type != IDEOBJ_QEXPR) {
        ideobj* key = keys;
        ideobj* value = values;

        if (strcmp(func, "def") == 0) {
            ideenv_global_put(env, key, value);
        }

        if (strcmp(func, "defl") == 0) {
            ideenv_put(env, key, value);
        }
    } else {
        for (int i=0; i<keys->count; i++) {
            ideobj* key = keys->cell[i];
            ideobj* value = values->cell[i];

            if (strcmp(func, "def") == 0) {
                ideenv_global_put(env, key, value);
            }

            if (strcmp(func, "defl") == 0) {
                ideenv_put(env, key, value);
            }
        }
    }


    ideobj_del(obj);
    return ideobj_sexpr();
}

ideobj* builtin_def(ideenv* env, ideobj* obj) {
    return builtin_var(env, obj, "def");
}

ideobj* builtin_defl(ideenv* env, ideobj* obj) {
    return builtin_var(env, obj, "defl");
}

ideobj* builtin_let(ideenv* env, ideobj* obj) {
    IASSERT_TYPE("let", obj, 0, IDEOBJ_QEXPR);
    IASSERT_TYPE("let", obj, 1, IDEOBJ_QEXPR);
    IASSERT_TYPE("let", obj, 2, IDEOBJ_QEXPR);

    IASSERT(
        obj,
        obj->cell[0]->count == obj->cell[1]->count,
        "let must recieve same number of keywords as values"
    );

    ideenv* local_env = ideenv_new_enclosed(env);

    for (int i=0; i<obj->cell[0]->count; i++) {
        IASSERT(
            obj,
            obj->cell[0]->cell[i]->type == IDEOBJ_SYMBOL,
            "Cannot define non-symbol, got %s, expected %s",
            idetype_name(obj->cell[0]->cell[i]->type),
            idetype_name(IDEOBJ_SYMBOL)
        );
    }

    for (int i=0; i<obj->cell[0]->count; i++) {
        ideenv_put(
            local_env,
            obj->cell[0]->cell[i],
            obj->cell[1]->cell[i]
        );
    }

    return builtin_eval(
        local_env,
        ideobj_list_add(
            ideobj_sexpr(),
            ideobj_copy(obj->cell[2])
        )
    );
}

ideobj* builtin_fn(ideenv* env, ideobj* obj) {
    IASSERT_NUM("fn", obj, 2);
    IASSERT_TYPE("fn", obj, 0, IDEOBJ_QEXPR);
    IASSERT_TYPE("fn", obj, 1, IDEOBJ_QEXPR);

    for (int i=0; i<obj->cell[0]->count; i++) {
        IASSERT(
            obj,
            obj->cell[0]->cell[i]->type == IDEOBJ_SYMBOL,
            "Cannot define non-symbol, got %s, expected %s",
            idetype_name(obj->cell[0]->cell[i]->type),
            idetype_name(IDEOBJ_SYMBOL)
        );
    }

    ideobj* params = ideobj_pop(obj, 0);
    ideobj* body = ideobj_pop(obj, 0);
    ideobj_del(obj);

    ideobj *fn = ideobj_fun(params, body);
    fn->env->parent = env;
    fn->env->depth = env->depth + 1;
    return fn;
}

ideobj* builtin_defn(ideenv* env, ideobj* obj) {
    IASSERT_NUM("defn", obj, 3);
    IASSERT_TYPE("defn", obj, 0, IDEOBJ_KEYWORD);
    IASSERT_TYPE("defn", obj, 1, IDEOBJ_QEXPR);
    IASSERT_TYPE("defn", obj, 2, IDEOBJ_QEXPR);

    ideobj* name = ideobj_pop(obj, 0);
    ideobj* params = ideobj_pop(obj, 0);
    ideobj* body = ideobj_pop(obj, 0);
    ideobj* fn = ideobj_fun(params, body);

    fn->env->parent = env;
    fn->env->depth = env->depth + 1;

    ideenv_global_put(env, name, fn);

    ideobj_del(obj);
    ideobj_del(fn);

    return ideobj_sexpr();
}

ideobj* builtin_exit(ideenv* env, ideobj* obj) {
    ideobj_del(obj);
    exit(0);
}

ideobj* builtin_print(ideenv* env, ideobj* obj) {
    for (int i=0; i<obj->count; i++) {
        ideobj_print(obj->cell[i]);
        putchar(' ');
    }

    ideobj_del(obj);
    putchar('\n');
    return ideobj_sexpr();
}

ideobj* builtin_add(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "+");
}

ideobj* builtin_sub(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "-");
}

ideobj* builtin_mul(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "*");
}

ideobj* builtin_div(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "/");
}

ideobj* builtin_mod(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "%");
}

ideobj* builtin_pow(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "^");
}

ideobj* builtin_min(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "min");
}

ideobj* builtin_max(ideenv* env, ideobj* obj) {
    return builtin_op(env, obj, "max");
}

ideobj* builtin_ord_num(ideenv *env, ideobj* obj, char* operator) {
    ideobj* left = ideobj_pop(obj, 0);
    ideobj* right = ideobj_pop(obj, 0);

    int status;
    if (strcmp(operator, ">") == 0) {
        status = left->num > right->num;
    }

    if (strcmp(operator, ">=") == 0) {
        status = left->num >= right->num;
    }

    if (strcmp(operator, "<") == 0) {
        status = left->num < right->num;
    }

    if (strcmp(operator, "<=") == 0) {
        status = left->num <= right->num;
    }

    ideobj_del(obj);
    return ideobj_num(status);
}

ideobj* builtin_ord_decimal(ideenv *env, ideobj* obj, char* operator) {
    ideobj* left = ideobj_pop(obj, 0);
    ideobj* right = ideobj_pop(obj, 0);

    double left_value = left->decimal;
    double right_value = right->decimal;

    if (left->type == IDEOBJ_NUM) {
        left_value = (double) left->num;
    }

    if (right->type == IDEOBJ_NUM) {
        right_value = (double) right->num;
    }

    int status;
    if (strcmp(operator, ">") == 0) {
        status = left_value > right_value;
    }

    if (strcmp(operator, ">=") == 0) {
        status = left_value >= right_value;
    }

    if (strcmp(operator, "<") == 0) {
        status = left_value < right_value;
    }

    if (strcmp(operator, "<=") == 0) {
        status = left_value <= right_value;
    }

    ideobj_del(obj);
    return ideobj_num(status);
}

ideobj* builtin_ord(ideenv *env, ideobj* obj, char* operator) {
    if (ideobj_cells_is_type(obj, IDEOBJ_NUM)) {
        return builtin_ord_num(env, obj, operator);
    }

    if (ideobj_is_numeric(obj->cell[0])) {
        return builtin_ord_decimal(env, obj, operator);
    }

    return ideobj_err("Cannot %s operate on non-number", operator);
}

ideobj* builtin_gt(ideenv* env, ideobj* obj) {
    return builtin_ord(env, obj, ">");
}

ideobj* builtin_gte(ideenv* env, ideobj* obj) {
    return builtin_ord(env, obj, ">=");
}

ideobj* builtin_lt(ideenv* env, ideobj* obj) {
    return builtin_ord(env, obj, "<");
}

ideobj* builtin_lte(ideenv* env, ideobj* obj) {
    return builtin_ord(env, obj, "<=");
}

ideobj* builtin_cmp(ideenv* env, ideobj* obj, char* operator) {
    IASSERT_NUM(operator, obj, 2);

    ideobj* left = ideobj_pop(obj, 0);
    ideobj* right = ideobj_pop(obj, 0);

    int status;
    if (strcmp(operator, "==") == 0) {
        status = ideobj_eq(left, right);
    }

    if (strcmp(operator, "!=") == 0) {
        status = ideobj_eq(left, right) == 0;
    }

    ideobj_del(obj);
    return ideobj_num(status);
}

ideobj* builtin_eq(ideenv* env, ideobj* obj) {
    return builtin_cmp(env, obj, "==");
}

ideobj* builtin_neq(ideenv* env, ideobj* obj) {
    return builtin_cmp(env, obj, "!=");
}

ideobj* builtin_if(ideenv* env, ideobj* obj) {
    IASSERT_NUM("if", obj, 3);
    IASSERT_TYPE("if", obj, 0, IDEOBJ_NUM);
    IASSERT_TYPE("if", obj, 1, IDEOBJ_QEXPR);
    IASSERT_TYPE("if", obj, 2, IDEOBJ_QEXPR);


    ideobj* condition = ideobj_pop(obj, 0);
    ideobj* consequence = ideobj_pop(obj, 0);
    ideobj* alternative = ideobj_pop(obj, 0);

    consequence->type = IDEOBJ_SEXPR;
    alternative->type = IDEOBJ_SEXPR;

    ideobj* result;
    if (ideobj_truthy(condition)) {
        result = ideobj_eval(env, consequence);
    } else {
        result = ideobj_eval(env, alternative);
    }

    ideobj_del(condition);
    ideobj_del(obj);
    return result;
}


ideobj* builtin_and(ideenv* env, ideobj* obj) {
    int status = 1;

    for (int i=0; i<obj->count; i++) {
        if (ideobj_truthy(obj->cell[i]) == 0) {
            status = 0;
            break;
        }
    }

    ideobj_del(obj);
    return ideobj_num(status);
}

ideobj* builtin_or(ideenv* env, ideobj* obj) {
    int status = 0;

    for (int i=0; i<obj->count; i++) {
        if (ideobj_truthy(obj->cell[i]) == 1) {
            status = 1;
            break;
        }
    }

    ideobj_del(obj);
    return ideobj_num(status);
}

ideobj* builtin_not(ideenv* env, ideobj* obj) {
    IASSERT_NUM("not", obj, 1);

    int status = !ideobj_truthy(obj->cell[0]);

    ideobj_del(obj);
    return ideobj_num(status);
}

ideobj* builtin_keyword(ideenv* env, ideobj* obj) {
    IASSERT_NUM("keyword", obj, 1);
    IASSERT_TYPE("keyword", obj, 0, IDEOBJ_STR);

    ideobj* keyword = ideobj_keyword(obj->cell[0]->str);
    ideobj_del(obj);
    return keyword;
}

ideobj* ideobj_read(mpc_ast_t* node);

ideobj* builtin_load(ideenv* env, ideobj *obj) {
    IASSERT_NUM("load", obj, 1);
    IASSERT_TYPE("load", obj, 0, IDEOBJ_STR);

    mpc_result_t result;
    if (mpc_parse_contents(obj->cell[0]->str, IdeLISP, &result)) {
        ideobj* expressions = ideobj_read(result.output);
        mpc_ast_delete(result.output);

        while (expressions->count) {
            ideobj* expression = ideobj_eval(env, ideobj_pop(expressions, 0));
            if (expression->type == IDEOBJ_ERR) {
              return expression;
            }
            ideobj_del(expression);
        }

        ideobj_del(expressions);
        ideobj_del(obj);
        return ideobj_sexpr();
    } else {
        char* result_err = mpc_err_string(result.error);

        ideobj* err = ideobj_err(
            "Could not load module %s, reason %s",
            obj->cell[0]->str,
            result_err
        );

        mpc_err_delete(result.error);
        free(result_err);
        ideobj_del(obj);

        return err;
    }
}

ideobj* ideobj_call_builtin(ideenv* env, ideobj* fun, ideobj* args) {
    return fun->builtin(env, args);
}

ideobj* ideobj_call_fun(ideenv* env, ideobj* fun, ideobj* args) {
    int fun_num_params = fun->params->count;
    int num_args = args->count;
    int has_zero_arity = 0;

    ideenv* fn_env = ideenv_copy(env);
    fn_env->parent = fun->env;

    // Allow calling zero arity functions with empty sexpr arg
    if (fun->params->count == 0) {
        if (
            args->count == 1 &&
            (
                args->cell[0]->type == IDEOBJ_SEXPR ||
                args->cell[0]->type == IDEOBJ_QEXPR
            ) &&
            args->cell[0]->count == 0
        ) {
            has_zero_arity = 1;
        }
    }

    while (args->count && !has_zero_arity) {
        if (fun->params->count == 0) {
            ideobj_del(args);

            return ideobj_err(
                "Function received too many arguments, expected %i, got %i",
                fun_num_params,
                num_args
            );
        }

        ideobj *param = ideobj_pop(fun->params, 0);

        if (strcmp(param->symbol, "&rest") == 0) {
            if (fun->params->count != 1) {
                ideobj_del(args);
                return ideobj_err(
                    "Invalid function format, &rest must be followed by symbol"
                );
            }


            ideobj *rest_param = ideobj_pop(fun->params, 0);
            ideenv_put(fn_env, rest_param, builtin_list(fn_env, args));
            ideobj_del(param);
            ideobj_del(rest_param);
            break;
        }

        ideobj *value = ideobj_pop(args, 0);

        ideenv_put(fn_env, param, value);
        ideobj_del(param);
        ideobj_del(value);
    }

    ideobj_del(args);

    if (fun->params->count == 0) {
        return builtin_eval(
            fn_env,
            ideobj_list_add(
                ideobj_sexpr(),
                ideobj_copy(fun->body)
            )
        );
    } else {
        fun->env = fn_env;
        return ideobj_copy(fun);
    }
}


ideobj* ideobj_eval_hashmap(ideenv* env, ideobj* obj) {
    for (int i=0; i<obj->count; i++) {
        obj->keys[i] = ideobj_eval(env, obj->keys[i]);
        obj->cell[i] = ideobj_eval(env, obj->cell[i]);
    }

    for (int i=0; i<obj->count; i++) {
        if (obj->keys[i]->type == IDEOBJ_ERR) {
            return ideobj_hashmap_take(obj, i, "key");
        }

        if (obj->cell[i]->type == IDEOBJ_ERR) {
            return ideobj_hashmap_take(obj, i, "value");
        }
    }

    return obj;
}

ideobj* ideobj_eval_sexpr(ideenv* env, ideobj* obj) {
    for (int i=0; i<obj->count; i++) {
        obj->cell[i] = ideobj_eval(env, obj->cell[i]);
    }

    for (int i=0; i<obj->count; i++) {
        if (obj->cell[i]->type == IDEOBJ_ERR) {
            return ideobj_take(obj, i);
        }
    }

    if (obj->count == 0) {
        return obj;
    }

    if (obj->count == 1) {
        return ideobj_eval(env, ideobj_take(obj, 0));
    }

    ideobj* first = ideobj_pop(obj, 0);

    if (first->type == IDEOBJ_ERR) {
        return first;
    }

    // Handle s-expressions wrapped in root
    if (first->type == IDEOBJ_SEXPR) {
        ideobj_del(obj);
        return first;
    }

    if (first->type != IDEOBJ_BUILTIN && first->type != IDEOBJ_FUN) {
        ideobj* err = ideobj_err(
            "Invalid first element, expected %s or %s, got %s",
            idetype_name(IDEOBJ_BUILTIN),
            idetype_name(IDEOBJ_FUN),
            idetype_name(first->type)
        );

        ideobj_del(first);
        ideobj_del(obj);
        return err;
    }

    if (first->type == IDEOBJ_BUILTIN) {
        return ideobj_call_builtin(env, first, obj);
    } else {
        return ideobj_call_fun(env, first, obj);
    }
}

ideobj* ideobj_eval(ideenv* env, ideobj* obj) {
    if (obj->type == IDEOBJ_SYMBOL) {
        ideobj* value = ideenv_get(env, obj);
        ideobj_del(obj);
        return value;
    }

    if (obj->type == IDEOBJ_SEXPR) {
        return ideobj_eval_sexpr(env, obj);
    }

    if (obj->type == IDEOBJ_HASHMAP) {
        return ideobj_eval_hashmap(env, obj);
    }
    return obj;
}

ideobj* ideobj_read_decimal(mpc_ast_t* node) {
    errno = 0;
    double decimal = strtod(node->contents, NULL);
    if (errno == ERANGE) {
        return ideobj_err("Invalid number");
    }
    return ideobj_decimal(decimal);
}

ideobj* ideobj_read_num(mpc_ast_t* node) {
    errno = 0;
    long num = strtol(node->contents, NULL, 10);
    if (errno == ERANGE) {
        return ideobj_err("Invalid number");
    }
    return ideobj_num(num);
}

ideobj* ideobj_read_string(mpc_ast_t* node) {
    node->contents[strlen(node->contents)-1] = '\0';

    char* unescaped = malloc(strlen(node->contents+1)+1);
    strcpy(unescaped, node->contents+1);

    unescaped = mpcf_unescape(unescaped);
    ideobj* obj = ideobj_str(unescaped);
    free(unescaped);
    return obj;
}

ideobj* ideobj_read_keyword(mpc_ast_t* node) {
    char* keyword = malloc(strlen(node->contents));
    strcpy(keyword, node->contents+1);

    ideobj* obj = ideobj_keyword(keyword);
    return obj;
}

ideobj* ideobj_read(mpc_ast_t* node) {
    if (strstr(node->tag, "decimal")) { return ideobj_read_decimal(node); }
    if (strstr(node->tag, "number")) { return ideobj_read_num(node); }
    if (strstr(node->tag, "string")) { return ideobj_read_string(node); }
    if (strstr(node->tag, "keyword")) { return ideobj_read_keyword(node); }
    if (strstr(node->tag, "symbol")) {
        return ideobj_symbol(node->contents);
    }

    ideobj* acc_value = NULL;
    if (strcmp(node->tag, ">") == 0) { acc_value = ideobj_sexpr(); }
    if (strstr(node->tag, "sexpr")) { acc_value = ideobj_sexpr(); }
    if (strstr(node->tag, "qexpr")) { acc_value = ideobj_qexpr(); }
    if (strstr(node->tag, "hashmap")) { acc_value = ideobj_hashmap(); }

    for (int i=0; i<node->children_num; i++) {
        if (strstr(node->children[i]->tag, "comment")) { continue; }
        if (strcmp(node->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "'(") == 0) { continue; }
        if (strcmp(node->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(node->children[i]->tag, "regex") == 0) { continue; }

        if (acc_value->type == IDEOBJ_HASHMAP) {
            acc_value = ideobj_hashmap_add(
                acc_value,
                ideobj_read(node->children[i]),
                ideobj_read(node->children[i+1])
            );
            i++;
        } else {
            acc_value = ideobj_list_add(acc_value, ideobj_read(node->children[i]));
        }

    }

    return acc_value;
}

void ideenv_add_builtin(ideenv* env, char* name, ibuiltin fn) {
    ideobj *key = ideobj_symbol(name);
    ideobj *fun = ideobj_builtin(fn);

    ideenv_put(env, key, fun);
    ideobj_del(key);
    ideobj_del(fun);
}

void ideenv_add_builtins(ideenv* env) {
    // List
    ideenv_add_builtin(env, "head", builtin_head);
    ideenv_add_builtin(env, "tail", builtin_tail);
    ideenv_add_builtin(env, "list", builtin_list);
    ideenv_add_builtin(env, "join", builtin_join);

    // Expression
    ideenv_add_builtin(env, "eval", builtin_eval);

    // Env
    ideenv_add_builtin(env, "def", builtin_def);
    ideenv_add_builtin(env, "defl", builtin_defl);
    ideenv_add_builtin(env, "let", builtin_let);

    // System
    ideenv_add_builtin(env, "exit", builtin_exit);
    ideenv_add_builtin(env, "print", builtin_print);
    ideenv_add_builtin(env, "load", builtin_load);
    ideenv_add_builtin(env, "error", builtin_error);
    ideenv_add_builtin(env, "type", builtin_type);
    ideenv_add_builtin(env, "len", builtin_len);

    // Functions
    ideenv_add_builtin(env, "fn", builtin_fn);
    ideenv_add_builtin(env, "defn", builtin_defn);

    // String
    ideenv_add_builtin(env, "concat", builtin_concat);
    ideenv_add_builtin(env, "str-split", builtin_str_split);
    ideenv_add_builtin(env, "str", builtin_str);
    ideenv_add_builtin(env, "upper-case", builtin_str_uppercase);
    ideenv_add_builtin(env, "lower-case", builtin_str_lowercase);

    // HashMap
    ideenv_add_builtin(env, "hash-map", builtin_hashmap);
    ideenv_add_builtin(env, "key", builtin_hashmap_key);
    ideenv_add_builtin(env, "assoc", builtin_hashmap_assoc);
    ideenv_add_builtin(env, "deassoc", builtin_hashmap_deassoc);

    // Operators
    ideenv_add_builtin(env, "+", builtin_add);
    ideenv_add_builtin(env, "-", builtin_sub);
    ideenv_add_builtin(env, "*", builtin_mul);
    ideenv_add_builtin(env, "/", builtin_div);
    ideenv_add_builtin(env, "%", builtin_mod);
    ideenv_add_builtin(env, "^", builtin_pow);
    ideenv_add_builtin(env, "min", builtin_min);
    ideenv_add_builtin(env, "max", builtin_max);

    // Comparisions
    ideenv_add_builtin(env, ">", builtin_gt);
    ideenv_add_builtin(env, ">=", builtin_gte);
    ideenv_add_builtin(env, "<", builtin_lt);
    ideenv_add_builtin(env, "<=", builtin_lte);
    ideenv_add_builtin(env, "==", builtin_eq);
    ideenv_add_builtin(env, "!=", builtin_neq);
    ideenv_add_builtin(env, "and", builtin_and);
    ideenv_add_builtin(env, "or", builtin_or);
    ideenv_add_builtin(env, "not", builtin_not);

    // Conditionals
    ideenv_add_builtin(env, "if", builtin_if);

    // Keyword
    ideenv_add_builtin(env, "keyword", builtin_keyword);
}

