#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include "mpc.h"

enum {
    IDEOBJ_ERR,
    IDEOBJ_NUM,
    IDEOBJ_SYMBOL,
    IDEOBJ_SEXPR,
    IDEOBJ_QEXPR,
    IDEOBJ_DECIMAL,
    IDEOBJ_BUILTIN,
    IDEOBJ_FUN
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
    ibuiltin builtin;

    ideenv* env;
    ideobj* params;
    ideobj* body;

    int count;
    struct ideobj** cell;
};

struct ideenv {
    ideenv* parent;
    int count;
    char** symbols;
    ideobj** values;
};


ideenv* ideenv_new(void);       // Forward declaration

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
    obj->symbol= malloc(strlen(symbol) + 1);
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

char* idetype_name(int type) {
    switch (type) {
        case IDEOBJ_ERR: return "Error";
        case IDEOBJ_SYMBOL: return "Symbol";
        case IDEOBJ_NUM: return "Number";
        case IDEOBJ_BUILTIN: return "Function";
        case IDEOBJ_QEXPR: return "Quoted Expression";
        case IDEOBJ_SEXPR: return "S-Expression";
        default: return "Unknown";
    }
}

void ideenv_del(ideenv* env);

void ideobj_del(ideobj* obj) {
    switch (obj->type) {
        case IDEOBJ_ERR: free(obj->err); break;
        case IDEOBJ_SYMBOL: free(obj->symbol); break;
        case IDEOBJ_NUM: break;
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
    }

    return copy;
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

void ideobj_print(ideobj* obj) {
    switch (obj->type) {
        case IDEOBJ_ERR:
            printf("Error: %s", obj->err);
            break;
        case IDEOBJ_NUM:
            printf("%li", obj->num);
            break;
        case IDEOBJ_DECIMAL:
            printf("%.2f", obj->decimal);
            break;
        case IDEOBJ_SYMBOL:
            printf("%s", obj->symbol);
            break;
        case IDEOBJ_SEXPR:
            ideobj_expr_print(obj, "(", ")");
            break;
        case IDEOBJ_QEXPR:
            ideobj_expr_print(obj, "'(", ")");
            break;
        case IDEOBJ_BUILTIN:
            printf("<function>");
            break;
        case IDEOBJ_FUN:
            printf("(fn ");
            ideobj_print(obj->params);
            putchar(' ');
            ideobj_print(obj->body);
            putchar(')');
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
    return env;
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
            puts(", ");
        }
        puts(env->symbols[i]);
        puts(": ");
        ideobj_print(env->values[i]);
    }
    putchar(')');
}


ideenv* ideenv_copy(ideenv* env) {
    ideenv* copy = malloc(sizeof(ideenv));
    copy->parent = env->parent;
    copy->count = env->count;
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
        if (strcmp(env->symbols[i], key->symbol) == 0) {
            ideobj_del(env->values[i]);
            env->values[i] = ideobj_copy(val);
            return;
        }
    }

    env->count++;
    env->values = realloc(env->values, sizeof(ideobj*) * env->count);
    env->symbols = realloc(env->symbols, sizeof(char*) * env->count);

    env->values[env->count - 1] = ideobj_copy(val);
    env->symbols[env->count - 1] = malloc(strlen(key->symbol) + 1);
    strcpy(env->symbols[env->count - 1], key->symbol);
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

ideobj* eval_op(ideobj* left, char* operator, ideobj* right) {
    if (left->type == IDEOBJ_ERR) { return left; };
    if (right->type == IDEOBJ_ERR) { return right; };

    if (left->type == IDEOBJ_NUM && right->type == IDEOBJ_NUM) {
        return eval_tenary_number_op(left, operator, right);
    }

    return ideobj_err("Invalid operator '%s'", operator);
}

ideobj* ideobj_add(ideobj* left, ideobj* right) {
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
    IASSERT(
        obj,
        obj->count == 1,
        "Function 'tail' received wrong number of arguments, requires 1"
    );
    IASSERT(
        obj,
        obj->cell[0]->type == IDEOBJ_QEXPR,
        "Function 'tail' received wrong type"
    );
    IASSERT(
        obj, obj->cell[0]->count != 0, "Function 'tail' received empty list"
    );

    ideobj* first = ideobj_take(obj, 0);
    ideobj_del(ideobj_pop(first, 0));
    return first;
}

ideobj* builtin_list(ideenv* env, ideobj* obj) {
    obj->type = IDEOBJ_QEXPR;
    return obj;
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
    while (right->count) {
        left = ideobj_add(left, ideobj_pop(right, 0));
    }

    ideobj_del(right);
    return left;
}

ideobj* builtin_join(ideenv* env, ideobj* obj) {
    for (int i=0; i<obj->count; i++) {
        IASSERT(
            obj,
            obj->cell[i]->type == IDEOBJ_QEXPR,
            "Function 'join' passed incorrect type"
        );
    }

    ideobj* first = ideobj_pop(obj, 0);
    while(obj->count) {
        first = ideobj_join(first, ideobj_pop(obj, 0));
    }

    return first;
}

ideobj* builtin_op(ideenv* env, ideobj* obj, char* operator) {
    for (int i=0; i<obj->count; i++) {
        if (obj->cell[i]->type != IDEOBJ_NUM) {
            ideobj_del(obj);
            return ideobj_err("Cannot operate on non-number");
        }
    }

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

ideobj* builtin_var(ideenv* env, ideobj* obj, char* func) {
    IASSERT_TYPE("fn", obj, 0, IDEOBJ_QEXPR);

    ideobj* keys = obj->cell[0];

    for (int i=0; i<keys->count; i++) {
        IASSERT(
            obj,
            keys->cell[i]->type == IDEOBJ_SYMBOL,
            "Function %s cannot set non-symbol",
            func
        );
    }

    IASSERT(
        obj,
        keys->count == obj->count-1,
        "set-v received incorrect number of names vs values"
    );

    for (int i=0; i<keys->count; i++) {
        ideobj* key = keys->cell[i];
        ideobj* value = obj->cell[i+1];

        if (strcmp(func, "def") == 0) {
            ideenv_global_put(env, key, value);
        }

        if (strcmp(func, "let") == 0) {
            ideenv_put(env, key, value);
        }
    }

    ideobj_del(obj);
    return ideobj_sexpr();
}

ideobj* builtin_def(ideenv* env, ideobj* obj) {
    return builtin_var(env, obj, "def");
}

ideobj* builtin_let(ideenv* env, ideobj* obj) {
    return builtin_var(env, obj, "let");
}

ideobj* builtin_fun(ideenv* env, ideobj* obj) {
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

    return ideobj_fun(params, body);
}

ideobj* builtin_exit(ideenv* env, ideobj* obj) {
    ideobj_del(obj);
    exit(0);
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

ideobj* ideobj_call_builtin(ideenv* env, ideobj* fun, ideobj* args) {
    return fun->builtin(env, args);
}

ideobj* ideobj_call_fun(ideenv* env, ideobj* fun, ideobj* args) {
    int fun_num_params = fun->params->count;
    int num_args = args->count;

    while (args->count) {
        if (fun->params->count == 0) {
            ideobj_del(args);

            return ideobj_err(
                "Function received too many arguments, expected %i, got %i",
                fun_num_params,
                num_args
            );
        }

        ideobj *arg = ideobj_pop(fun->params, 0);
        ideobj *value = ideobj_pop(args, 0);

        ideenv_put(fun->env, arg, value);
        ideobj_del(arg);
        ideobj_del(value);
    }

    ideobj_del(args);

    if (fun->params->count == 0) {
        fun->env->parent = env;

        return builtin_eval(
            fun->env,
            ideobj_add(
                ideobj_sexpr(),
                ideobj_copy(fun->body)
            )
        );
    } else {
        return ideobj_copy(fun);
    }
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
        return ideobj_take(obj, 0);
    }

    ideobj* first = ideobj_pop(obj, 0);

    if (first->type == IDEOBJ_ERR) {
        return first;
    }

    if (first->type != IDEOBJ_BUILTIN && first->type != IDEOBJ_FUN) {
        ideobj_del(first);
        ideobj_del(obj);
        return ideobj_err(
            "Invalid first element, expected %s, got %s",
            idetype_name(IDEOBJ_BUILTIN),
            idetype_name(first->type)
        );
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
    return obj;
}

ideobj* ideobj_read_num(mpc_ast_t* node) {
    errno = 0;
    long num = strtol(node->contents, NULL, 10);
    if (errno == ERANGE) {
        return ideobj_err("Invalid number");
    }
    return ideobj_num(num);
}

ideobj* ideobj_read(mpc_ast_t* node) {
    if (strstr(node->tag, "number")) { return ideobj_read_num(node); }
    if (strstr(node->tag, "symbol")) {
        return ideobj_symbol(node->contents);
    }

    ideobj* acc_value = NULL;
    if (strcmp(node->tag, ">") == 0) { acc_value = ideobj_sexpr(); }
    if (strstr(node->tag, "sexpr")) { acc_value = ideobj_sexpr(); }
    if (strstr(node->tag, "qexpr")) { acc_value = ideobj_qexpr(); }

    for (int i=0; i<node->children_num; i++) {
        if (strcmp(node->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "'(") == 0) { continue; }
        if (strcmp(node->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(node->children[i]->tag, "regex") == 0) { continue; }

        acc_value = ideobj_add(acc_value, ideobj_read(node->children[i]));
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
    ideenv_add_builtin(env, "head", builtin_head);
    ideenv_add_builtin(env, "tail", builtin_tail);
    ideenv_add_builtin(env, "list", builtin_list);
    ideenv_add_builtin(env, "eval", builtin_eval);
    ideenv_add_builtin(env, "join", builtin_join);
    ideenv_add_builtin(env, "def", builtin_def);
    ideenv_add_builtin(env, "let", builtin_let);
    ideenv_add_builtin(env, "exit", builtin_exit);
    ideenv_add_builtin(env, "fn", builtin_fun);

    ideenv_add_builtin(env, "+", builtin_add);
    ideenv_add_builtin(env, "-", builtin_sub);
    ideenv_add_builtin(env, "*", builtin_mul);
    ideenv_add_builtin(env, "/", builtin_div);
    ideenv_add_builtin(env, "%", builtin_mod);
    ideenv_add_builtin(env, "^", builtin_pow);
    ideenv_add_builtin(env, "min", builtin_min);
    ideenv_add_builtin(env, "max", builtin_max);
}

int main(int argc, char** argv) {
    puts("IdeLISP (type exit() to quit)");

    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Symbol = mpc_new("symbol");
    mpc_parser_t* Sexpr = mpc_new("sexpr");
    mpc_parser_t* Qexpr = mpc_new("qexpr");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* IdeLISP = mpc_new("idelisp");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                     \
            number   : /-?[0-9]+/ ;                                           \
            symbol   : /[a-zA-Z0-9_+^\\-*\\/\\\\=<>!&]+/ ;                    \
            sexpr    : '(' <expr>* ')' ;                                      \
            qexpr    : '{' <expr>* '}' | \"'(\" <expr>* ')' ;                 \
            expr     : <number> | <symbol> | <sexpr> | <qexpr> ;              \
            idelisp  : /^/ <expr>* /$/ ;                                      \
        "
        , Number, Symbol, Sexpr, Qexpr, Expr, IdeLISP);

    ideenv* env = ideenv_new();
    ideenv_add_builtins(env);

    while(1) {
        char* source = readline(">> ");
        add_history(source);

        mpc_result_t result;
        if (mpc_parse("<stdin>", source, IdeLISP, &result)) {
            mpc_ast_t* root_node = result.output;

            ideobj* v = ideobj_eval(
                env,
                ideobj_read(root_node)
            );
            ideobj_println(v);
            ideobj_del(v);
        } else {
            mpc_err_print(result.error);
        }
        mpc_ast_delete(result.output);

        free(source);
    }

    ideenv_del(env);

    mpc_cleanup(5, Number, Symbol, Sexpr, Qexpr, Expr, IdeLISP);
    return 0;
}
