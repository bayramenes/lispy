#include <stdio.h>  
#include <stdlib.h>
#include "mpc.h"
#include <string.h>
#include <math.h>
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

// Function Prototypes
int evaluate(mpc_ast_t * t);
int evaluate_operation(long x , char* op, long y);
int number_of_nodes(mpc_ast_t * t);
// buffer for user input
static char input[2048];



/*
BONUS EXERCISES:
1. Add the ability to use the exponentiation operator using the ^ symbol.
2. Add the ability to use the modulo operator using the % symbol.
3. Add the ability to use the max and min functions.
4. Count the number of nodes in a given AST using a new function int number_of_nodes(mpc_ast_t* t).
5 Count the maximum number of branches of a single node in an AST using a new function int max_number_of_children(mpc_ast_t* t).
6. Add a new function int max(int a, int b) that returns the maximum of two integers.
7. Add support for textual operator names (add, sub, mul, div, mod, exp, max, min).
*/

int main(int argc, char** argv) {

    // create a parser for polish notation
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lispy = mpc_new("lispy");


    // define the syntax
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                                                                                                               \
    number      : /-?(([0-9]+\\.[0-9]+)|([0-9]+))/ ;                                                                                                \
    operator    : \"max\" | \"min\" | '+' | \"add\" | '-' | \"sub\" | '*' | \"mul\" | '/' | \"div\" | '%' | \"mod\" | '^' | \"exp\" ;               \
    expr        : <number> | '(' <expr> <operator> <expr> ')' ;                                                                                     \
    lispy       : /^/ <expr> <operator> <expr> /$/ ;                                                                                                \
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
            // on success evaluate the expression and print the result
            long result = evaluate(r.output);
            printf("%li\n", result);
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




int evaluate(mpc_ast_t * t){
    // if tagged as number return the value
    if (strstr(t->tag, "number")) {
        return atoi(t->contents);
    }

    /*
    in an expression child order is:
    0: '('
    1: <expr>
    2: <operator>
    3: <expr>
    4: ')'
    */
    

    // get the operator 
    char * operation = t->children[2]->contents;

    // get the value of the first and second expressions

    long first = evaluate(t->children[1]);
    long second = evaluate(t->children[3]);

    // evaluate
    return evaluate_operation(first, operation, second);

}


int evaluate_operation(long x , char* op, long y){
    if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0) {
        return x + y;
    }
    if (strcmp(op, "-" ) == 0 || strcmp(op, "sub") == 0) {
        return x - y;
    }
    if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) {
        return x * y;
    }
    if (strcmp(op, "/") == 0 || strcmp(op, "div") == 0) {
        return x / y;
    }
    if (strcmp(op, "^") == 0 || strcmp(op, "exp") == 0) {
        return pow(x, y);
    }
    if (strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) {
        return x % y;
    }
    if (strcmp(op, "min") == 0) {
        return x < y ? x : y;
    }
    if (strcmp(op, "max") == 0) {
        return x > y ? x : y;
    }
    return 0;
}


int number_of_nodes(mpc_ast_t * t) {
    if (t->children_num == 0) {
        return 1;
    }
    if (t->children_num >= 1) {
        int total = 1;
        for (int i = 0; i < t->children_num; i++) {
            total += number_of_nodes(t->children[i]);
        }
        return total;
    }
    return 0;
}



// most number of children from one branch
int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int max_number_of_children(mpc_ast_t * t){
    if (t->children_num == 0) {
        return 0;
    }
    
    int max_children = 0;
    for (int i = 0; i < t->children_num; i++) {
        max_children = max(max_children, max_number_of_children(t->children[i]));
    }
    return max_children + 1;
}

