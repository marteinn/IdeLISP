#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include "mpc.h"

enum { IDEOBJ_NUM, IDEOBJ_DECIMAL, IDEOBJ_ERR };
enum {
    IDERR_DIV_ZERO, IDERR_BAD_OP, IDERR_BAD_NUM, IDERR_MODULO_BY_ZERO
};

typedef struct {
    int type;
    long num;
    double decimal;
    int err;
} ideobj;

ideobj ideobj_num(long val) {
    ideobj obj;

    obj.type = IDEOBJ_NUM;
    obj.num = val;
    return obj;
}

ideobj ideobj_decimal(double val) {
    ideobj obj;

    obj.type = IDEOBJ_DECIMAL;
    obj.decimal = val;
    return obj;
}

ideobj ideobj_err(int err) {
    ideobj obj;

    obj.type = IDEOBJ_ERR;
    obj.err = err;
    return obj;
}

void ideobj_print(ideobj obj) {
    switch (obj.type) {
        case IDEOBJ_NUM:
            printf("%li", obj.num);
            break;
        case IDEOBJ_DECIMAL:
            printf("%.2f", obj.decimal);
            break;
        case IDEOBJ_ERR:
            if (obj.err == IDERR_DIV_ZERO) {
                printf("Error: Division by zero");
            }
            if (obj.err == IDERR_BAD_OP) {
                printf("Error: Invalid operator");
            }
            if (obj.err == IDERR_MODULO_BY_ZERO) {
                printf("Error: Modulo by zero");
            }
            break;
    }
}

void ideobj_println(ideobj obj) {
    ideobj_print(obj);
    putchar('\n');
}

ideobj eval_tenary_number_op(ideobj left, char* operator, ideobj right) {
    if (strcmp(operator, "+") == 0) {
        return ideobj_num(left.num + right.num);
    }
    if (strcmp(operator, "-") == 0) {
        return ideobj_num(left.num - right.num);
    }
    if (strcmp(operator, "*") == 0) {
        return ideobj_num(left.num * right.num);
    }
    if (strcmp(operator, "/") == 0) {
        if (right.num == 0) {
            return ideobj_err(IDERR_DIV_ZERO);
        }
        return ideobj_num(left.num / right.num);
    }
    if (strcmp(operator, "%") == 0) {
        if (right.num == 0) {
            return ideobj_err(IDERR_MODULO_BY_ZERO);
        }
        return ideobj_num(left.num % right.num);
    }
    if (strcmp(operator, "^") == 0) {
        return ideobj_num(pow(left.num, right.num));
    }
    if (strcmp(operator, "min") == 0) {
        return ideobj_num(
            left.num < right.num ? left.num : right.num
        );
    }
    if (strcmp(operator, "max") == 0) {
        return ideobj_num(left.num > right.num ? left.num : right.num);
    }

    return ideobj_err(IDERR_BAD_OP);
}

ideobj eval_op(ideobj left, char* operator, ideobj right) {
    if (left.type == IDEOBJ_ERR) { return left; };
    if (right.type == IDEOBJ_ERR) { return right; };

    if (left.type == IDEOBJ_NUM && right.type == IDEOBJ_NUM) {
        return eval_tenary_number_op(left, operator, right);
    }

    return ideobj_err(IDERR_BAD_OP);
}

ideobj eval(mpc_ast_t* node) {
    if (strstr(node->tag, "number")) {
        errno = 0;
        long num = strtol(node->contents, NULL, 10);
        if (errno == ERANGE) {
            return ideobj_err(IDERR_BAD_NUM);
        }
        return ideobj_num(num);
    }

    char* operator = node->children[1]->contents;
    ideobj acc_value = eval(node->children[2]);
    int node_children = node->children_num - 3;

    int i = 3;
    while(strstr(node->children[i]->tag, "expr")) {
        acc_value = eval_op(acc_value, operator, eval(node->children[i]));
        i++;
    }

    // Negate value if only one child is passed
    if (strcmp(operator, "-") == 0 && node_children == 1) {
        return ideobj_num(-acc_value.num);
    }

    return acc_value;
}

int main(int argc, char** argv) {
    puts("IdeLISP (type exit() to quit)");

    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* IdeLISP = mpc_new("idelisp");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                     \
            number   : /-?[0-9]+/ ;                                           \
            operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;\
            expr     : <number> | '(' <operator> <expr>+ ')' ;                \
            idelisp  : /^/ <operator> <expr>+ /$/ ;                           \
        "
        , Number, Operator, Expr, IdeLISP);

    while(1) {
        char* source = readline(">> ");
        add_history(source);

        mpc_result_t result;
        if (mpc_parse("<stdin>", source, IdeLISP, &result)) {
            mpc_ast_t* root_node = result.output;

            ideobj v = eval(root_node);
            ideobj_println(v);
        } else {
            mpc_err_print(result.error);
        }
        mpc_ast_delete(result.output);

        free(source);
    }

    mpc_cleanup(4, Number, Operator, Expr, IdeLISP);
    return 0;
}
