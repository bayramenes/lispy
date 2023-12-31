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



// struct for error handling

// lval stands for lispy value
typedef struct {
    int type;
    long num;
    int err;    
}lval;




// ENUMS

// to differentiate bewteen error and number from the obtained result
enum { LVAL_NUM, LVAL_ERR };

// to differentiate between the type of error
/*
1. Division by zero
2. Invalid operator
3. Invalid number (too big)
*/
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };





// Function Prototypes
lval evaluate(mpc_ast_t * t);
lval evaluate_operation(lval x , char* op, lval y);
int number_of_nodes(mpc_ast_t * t);
void print_stuff(mpc_ast_t * t);
lval lval_num(long x);
lval lval_err(int x);
void lval_print(lval v);








/*
BONUS EXERCISES:
*/


// buffer for user input
static char input[2048];


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
    expr        : <number> | '(' <operator> <expr>+')' ;                                                                                     \
    lispy       : /^/ <operator> <expr>+ /$/ ;                                                                                            \
    ",
    Number, Operator, Expr, Lispy);

    // do forever
    while (1) {

        // get the user input
        char* input = readline("lispy> ");
        add_history(input);

        // exit
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // clear screen
        if (strcmp(input, "clear") == 0) {
            printf("\e[1;1H\e[2J");
            continue;
        }
        

        // try to parse the user input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            // on success evaluate the expression and print the result
            // the result might be an error or a number
            lval result = evaluate(r.output);
            lval_print(result);
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

// function to print the number or the proper error message
void lval_print(lval v) {
    switch (v.type) {
    // if the type is a number print it
    case LVAL_NUM: printf("%li\n", v.num); break;
    // if the type is an error print the error message
    case LVAL_ERR:
        if (v.err == LERR_DIV_ZERO) { printf("Error: Division by zero!\n"); }
        if (v.err == LERR_BAD_OP) { printf("Error: Invalid operator!\n"); }
        if (v.err == LERR_BAD_NUM) { printf("Error: Invalid number!\n"); }
        break;
    }
}




lval evaluate(mpc_ast_t * t){
    // if tagged as number return the value
    // try to convert the contents to a number
    // if too big or too small return an error
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    /*
    in an expression child order is:
    0: '('
    1: <operator>
    2: <expr>
    3: <expr>
    ...
    last: ')'
    */
    

    // get the operator 
    char * operation = t->children[1]->contents;

    // get the value of the first and second expressions

    lval first = evaluate(t->children[2]);
    int i = 3;
    // iterate over the remaining children and combine the result
    while (strstr(t->children[i]->tag, "expr")) {
        first = evaluate_operation(first, operation, evaluate(t->children[i]));
        i++;
    }

    return first;

}

lval evaluate_operation(lval x , char* op, lval y){

    // if any of the numbers is an error return it
    if (x.type == LVAL_ERR) { return x; }
    if (y.type == LVAL_ERR) { return y; }


    // check the operator and perform the operation
    if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0) {return lval_num(x.num + y.num);}
    if (strcmp(op, "-" ) == 0 || strcmp(op, "sub") == 0) {return lval_num(x.num - y.num);}
    if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) {return lval_num(x.num * y.num);}
    // for the division we have to check for division by zero
    if (strcmp(op, "/") == 0 || strcmp(op, "div") == 0) {
        return y.num != 0 ? lval_num(x.num / y.num) : lval_err(LERR_DIV_ZERO);
    }

    if (strcmp(op, "^") == 0 || strcmp(op, "exp") == 0) {return lval_num(pow(x.num, y.num));}
    if (strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) {return lval_num(x.num % y.num);}
    if (strcmp(op, "min") == 0) {return x.num < y.num ? x : y;}
    if (strcmp(op, "max") == 0) {return x.num > y.num ? x : y; }

    // if none match then return an error
    return lval_err(LERR_BAD_OP);
}


// functions to create lval for evaluation



// here x is the actual number values we want
lval lval_num(long x){
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

// whereas here x is the error type which is defined in the enum
// so when calling this function we will give it an enum value as an argument and it will be
// interpreted as an integer 
lval lval_err(int x){
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}



