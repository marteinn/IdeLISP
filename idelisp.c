#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include "mpc.h"

int main(int argc, char** argv) {
    puts("IdeLISP (type exit() to quit)");

    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* IdeLISP = mpc_new("idelisp");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                     \
            number   : /-?[0-9]+/ ;                                           \
            operator : '+' | '-' | '*' | '/' ;                                \
            expr     : <number> | '(' <operator> <expr>+ ')' ;                \
            idelisp  : /^/ <operator> <expr>+ /$/ ;                           \
        "
        , Number, Operator, Expr, IdeLISP);

    while(1) {
        char* source = readline(">> ");
        add_history(source);

        mpc_result_t result;
        if (mpc_parse("<stdin>", source, IdeLISP, &result)) {
            mpc_ast_print(result.output);
        } else {
            mpc_err_print(result.error);
        }
        mpc_ast_delete(result.output);

        printf("%s\n", source);
        free(source);
    }

    mpc_cleanup(4, Number, Operator, Expr, IdeLISP);
    return 0;
}
