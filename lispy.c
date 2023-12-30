#include <stdio.h>  
#include <stdlib.h>
#include "mpc.h"
#include <string.h>

// if we are compiling on Windows compile these functions
#ifdef _WIN32
static char buffer[2048];
// fake readline function
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    // copy and return the input

    // +1 for the null terminator
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    // free the memory
    // add null terminator
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}
// fake add_history function
// no need to do anything it is already handeled by windows
void add_history(char* unused) {}
// otherwise include the editline headers
#else
#include <editline/readline.h>
#endif

// buffer for user input
static char input[2048];


/*
CHAPTER 6
BONUS STUFF I DID
1. support for decimal numbers
2. support for the modulo operator
3. changed the grammar from polish notation to conventional notation
*/

int main(int argc, char** argv) {

    // create a parser for polish notation
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");


    // define the syntax
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                       \
    number      : /-?(([0-9]+\\.[0-9]+)|([0-9]+))/ ;                              \
    operator    : '+' | '-' | '*' | '/' | '%' ;             \
    expr        : <number> | '(' <expr> <operator> <expr> ')' ;   \
    lispy       : /^/ <expr> <operator> <expr> /$/ ;              \
    ",
    Number, Operator, Expr, Lispy);

    // do forever
    while (1) {

        // get the user input
        char* input = readline("lispy> ");
        add_history(input);


        if (strcmp(input, "exit") == 0) {
            break;
        }

        // try to parse the user input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // on success print the AST
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        }
        else {
            // otherwise print the error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        // free the memory
        free(input);
    }

    // clean up the parser
    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;

}