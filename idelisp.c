#include "core.c"
#include <editline/readline.h>

enum { RUNMODE_REPL, RUNMODE_FILE };

int main(int argc, char** argv) {
    int run_mode = RUNMODE_REPL;
    char* source_file = NULL;

    for (int i=0; i<argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i<argc-1) {
            source_file = argv[i+1];
            run_mode = RUNMODE_FILE;
            i++;
        }
    }

    Decimal = mpc_new("decimal");
    Number = mpc_new("number");
    String = mpc_new("string");
    Symbol = mpc_new("symbol");
    Keyword = mpc_new("keyword");
    Comment = mpc_new("comment");
    HashMap = mpc_new("hashmap");
    Sexpr = mpc_new("sexpr");
    Qexpr = mpc_new("qexpr");
    Expr = mpc_new("expr");
    IdeLISP = mpc_new("idelisp");

    mpca_lang(MPCA_LANG_DEFAULT,
        "                                                                     \
            decimal  : /-?[0-9]+\\.[0-9]+/ ;                                  \
            number   : /-?[0-9]+/ ;                                           \
            string   : /\"(\\\\.|[^\"])*\"/ ;                                 \
            keyword  : /:[a-zA-Z0-9_+^\\-*\\/\\\\=<>!&%\\?]+/ ;               \
            symbol   : /[a-zA-Z0-9_+^\\-*\\/\\\\=<>!&%\\?]+/ ;                \
            comment  : /;[^\\r\\n]*/ ;                                        \
            hashmap  : '{' <expr>* '}' ;                                      \
            sexpr    : '(' <expr>* ')' ;                                      \
            qexpr    : \"'(\" <expr>* ')' ;                                   \
            expr     : <decimal> | <number> | <string> | <keyword> | <symbol> \
                     | <sexpr> | <qexpr> | <comment> | <hashmap> ;            \
            idelisp  : /^/ <expr>* /$/ ;                                      \
        ",
        Decimal,
        Number,
        String,
        Keyword,
        Symbol,
        Comment,
        HashMap,
        Sexpr,
        Qexpr,
        Expr,
        IdeLISP);

    ideenv* env = ideenv_new();
    env->depth = 0;
    ideenv_add_builtins(env);

    if (run_mode == RUNMODE_FILE) {
        ideobj* args = ideobj_list_add(ideobj_sexpr(), ideobj_str(source_file));
        ideobj* expression = builtin_load(env, args);

        if (expression->type == IDEOBJ_ERR) {
            ideobj_println(expression);
            return 1;
        }
        ideobj_del(expression);
        return 0;
    }

    puts("IdeLISP (type exit() to quit)");
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

    mpc_cleanup(
        11,
        Decimal,
        Number,
        String,
        Keyword,
        Symbol,
        Comment,
        HashMap,
        Sexpr,
        Qexpr,
        Expr,
        IdeLISP
    );
    return 0;
}
