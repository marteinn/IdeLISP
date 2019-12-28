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
    IDEOBJ_DECIMAL
};

typedef struct ideobj {
    int type;
    long num;
    double decimal;
    char* err;
    char* symbol;
    // expressions
    int count;
    struct ideobj** cell;
} ideobj;

ideobj* ideobj_err(char* err) {
    ideobj* obj = malloc(sizeof(ideobj));

    obj->type = IDEOBJ_ERR;
    obj->err = malloc(strlen(err) + 1);
    strcpy(obj->err, err);
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

void ideobj_del(ideobj* obj) {
    switch (obj->type) {
        case IDEOBJ_ERR: free(obj->err); break;
        case IDEOBJ_SYMBOL: free(obj->symbol); break;
        case IDEOBJ_NUM: break;
        case IDEOBJ_QEXPR:
        case IDEOBJ_SEXPR:
            for (int i=0; i<obj->count; i++) {
                ideobj_del(obj->cell[i]);
            }
            free(obj->cell);
    }

    free(obj);
}

void ideobj_print(ideobj* obj);

void ideobj_expr_print(ideobj* obj, char open, char close) {
    putchar(open);

    for (int i=0; i<obj->count; i++) {
        if (i > 0) {
            putchar(' ');
        }
        ideobj_print(obj->cell[i]);
    }

    putchar(close);
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
            ideobj_expr_print(obj, '(', ')');
            break;
        case IDEOBJ_QEXPR:
            ideobj_expr_print(obj, '{', '}');
            break;
    }
}

void ideobj_println(ideobj* obj) {
    ideobj_print(obj);
    putchar('\n');
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

    return ideobj_err("Invalid operator");
}

ideobj* eval_op(ideobj* left, char* operator, ideobj* right) {
    if (left->type == IDEOBJ_ERR) { return left; };
    if (right->type == IDEOBJ_ERR) { return right; };

    if (left->type == IDEOBJ_NUM && right->type == IDEOBJ_NUM) {
        return eval_tenary_number_op(left, operator, right);
    }

    return ideobj_err("Invalid operator");
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

#define IASSERT(args, cond, err) \
    if (!(cond)) { ideobj_del(args); return ideobj_err(err); }

ideobj* ideobj_eval(ideobj* obj);

ideobj* builtin_head(ideobj* obj) {
    IASSERT(
        obj,
        obj->count == 1,
        "Function 'head' received wrong number of arguments, requires 1"
    );
    IASSERT(
        obj,
        obj->cell[0]->type == IDEOBJ_QEXPR,
        "Function 'head' received wrong type"
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

ideobj* builtin_tail(ideobj* obj) {
    puts("builtin_tail");

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

ideobj* builtin_list(ideobj* obj) {
    obj->type = IDEOBJ_QEXPR;
    return obj;
}

ideobj* builtin_eval(ideobj* obj) {
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
    return ideobj_eval(first);
}

ideobj* ideobj_join(ideobj* left, ideobj* right) {
    while (right->count) {
        left = ideobj_add(left, ideobj_pop(right, 0));
    }

    ideobj_del(right);
    return left;
}

ideobj* builtin_join(ideobj* obj) {
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


ideobj* builtin_op(ideobj* obj, char* operator) {
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

ideobj* builtin(ideobj* obj, char* operator) {
    if(strcmp("list", operator) == 0) { return builtin_list(obj); }
    if(strcmp("head", operator) == 0) { return builtin_head(obj); }
    if(strcmp("tail", operator) == 0) { return builtin_tail(obj); }
    if(strcmp("join", operator) == 0) { return builtin_join(obj); }
    if(strcmp("eval", operator) == 0) { return builtin_eval(obj); }

    if(strstr("+-/*%^", operator)) {
        return builtin_op(obj, operator);
    }
    ideobj_del(obj);
    return ideobj_err("Invalid operator");
}

ideobj* ideobj_eval_sexpr(ideobj* obj) {
    for (int i=0; i<obj->count; i++) {
        obj->cell[i] = ideobj_eval(obj->cell[i]);
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
    if (first->type != IDEOBJ_SYMBOL) {
        ideobj_del(first);
        ideobj_del(obj);
        return ideobj_err("S-Expression does not start with symbol");
    }

    ideobj* result = builtin(obj, first->symbol);
    ideobj_del(first);
    return result;
}

ideobj* ideobj_eval(ideobj* obj) {
    if (obj->type == IDEOBJ_SEXPR) {
        return ideobj_eval_sexpr(obj);
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
        if (strcmp(node->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(node->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(node->children[i]->tag, "regex") == 0) { continue; }

        acc_value = ideobj_add(acc_value, ideobj_read(node->children[i]));
    }

    return acc_value;
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
            symbol   : \"list\" | \"head\" | \"tail\" | \"join\" | \"eval\"   \
                     | '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;\
            sexpr    : '(' <expr>* ')' ;                                      \
            qexpr    : '{' <expr>* '}' ;                                      \
            expr     : <number> | <symbol> | <sexpr> | <qexpr> ;              \
            idelisp  : /^/ <expr>* /$/ ;                                      \
        "
        , Number, Symbol, Sexpr, Qexpr, Expr, IdeLISP);

    while(1) {
        char* source = readline(">> ");
        add_history(source);

        mpc_result_t result;
        if (mpc_parse("<stdin>", source, IdeLISP, &result)) {
            mpc_ast_t* root_node = result.output;

            ideobj* v = ideobj_eval(
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

    mpc_cleanup(5, Number, Symbol, Sexpr, Qexpr, Expr, IdeLISP);
    return 0;
}
