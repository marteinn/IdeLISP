#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include "mpc.h"

long eval_op(long left_value, char* operator, long right_value) {
    if (strcmp(operator, "+") == 0) { return left_value + right_value; }
    if (strcmp(operator, "-") == 0) { return left_value - right_value; }
    if (strcmp(operator, "*") == 0) { return left_value * right_value; }
    if (strcmp(operator, "/") == 0) { return left_value / right_value; }
    if (strcmp(operator, "%") == 0) { return left_value % right_value; }
    if (strcmp(operator, "^") == 0) { return pow(left_value, right_value); }
    if (strcmp(operator, "min") == 0) {
        return left_value < right_value ? left_value : right_value;
    }
    if (strcmp(operator, "max") == 0) {
        return left_value > right_value ? left_value : right_value;
    }
    return 0;
}

long eval(mpc_ast_t* node) {
    if (strstr(node->tag, "number")) {
        return atoi(node->contents);
    }

    char* operator = node->children[1]->contents;
    long acc_value = eval(node->children[2]);
    int node_children = node->children_num - 3;

    int i = 3;
    while(strstr(node->children[i]->tag, "expr")) {
        acc_value = eval_op(acc_value, operator, eval(node->children[i]));
        i++;
    }

    // Negate value if only one child is passed
    if (strcmp(operator, "-") == 0 && node_children == 1) {
        return -acc_value;
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

            long v = eval(root_node);
            printf("%li\n", v);
        } else {
            mpc_err_print(result.error);
        }
        mpc_ast_delete(result.output);

        free(source);
    }

    mpc_cleanup(4, Number, Operator, Expr, IdeLISP);
    return 0;
}
