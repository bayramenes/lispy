#include <stdio.h>  
#include <stdlib.h>
#include "mpc.h"

// if we are compiling on Windows compile these functions
#ifdef _WIN32
#include <string.h>
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



int main(int argc, char** argv) {

    // do forever
    while (1) {

        // get the user input
        char* input = readline("lispy> ");
        add_history(input);
        // print the input
        printf("No you're a %s\n", input);
        // free the memory
        free(input);
    }

    return 0;

}