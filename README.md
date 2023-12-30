This is a repo documenting my journey through the book "Build your own lisp"

In this book we learn how to create a basic interpreter with c and build a lisp using parsers,lexers etc...

i will updating each chapter i commit and i will write down the bonus stuff i did (if any) (NOTE: bonus stuff are things written in the book after each chapter more like an exercise)


I will start documenting from Chapter 6 since chapters before aren't really a thing but more like introduction to C and so on...

###

## CHAPTER 6 BONUS:
1. support for decimal numbers
2. support for the modulo operator
3. changed the grammar from polish notation to conventional notation



## CHAPTER 7 BONUS:
1. Add the ability to use the exponentiation operator using the ^ symbol.
2. Add the ability to use the modulo operator using the % symbol.
3. Add the ability to use the max and min functions.
4. Count the number of nodes in a given AST using a new function int number_of_nodes(mpc_ast_t* t).
5. Count the maximum number of branches of a single node in an AST using a new function int max_number_of_children(mpc_ast_t* t).
6. Add a new function int max(int a, int b) that returns the maximum of two integers.
7. Add support for textual operator names (add, sub, mul, div, mod, exp, max, min).