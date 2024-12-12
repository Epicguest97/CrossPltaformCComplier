# Simple C Compiler
This is a basic C compiler that demonstrates the core concepts of compiler design. It processes a very simple subset of the C language and generates x86 assembly code for a minimal program that returns an integer from main(). This project focuses on the key stages of a compiler: Lexical Analysis, Syntax Analysis, and Code Generation.

# Key Features
- Lexer (Tokenization):
- Breaks the source code into tokens.
- Recognizes basic components like keywords, identifiers, integers, and punctuation.
- Tracks line and column numbers for better debugging (though error handling is minimal).
- Parser:
- Creates an Abstract Syntax Tree (AST) for simple function declarations.
- Parses return statements with integer literals.
# Code Generator:
Outputs x86 assembly code for simple programs.
Supports very basic C syntax: an integer return from the main() function.
## Example Usage
1. Create a simple C file (example.c):
```bash
int main() {
    return 42;
}
```
2. Compile the compiler itself:
```bash
gcc simple_compiler.c -o simple_compiler
```
3. Use the compiler to generate an assembly file from your C code:
```bash
./simple_compiler example.c output.s
```
4. Compile the assembly into an executable:
```bash
gcc output.s -o example
```
5. Run the program:
```bash
./example
```
6. This should output 42 (the value returned from main()).

Check the exit status:
```bash
echo $?
```
This will print 42, which is the return value from the main() function.

# Compiler Stages Demonstrated
- Lexical Analysis (Tokenization): The lexer breaks the C source code into a stream of tokens, such as keywords, identifiers, and operators.

- Syntax Analysis (Parsing): The parser converts the token stream into an Abstract Syntax Tree (AST). It currently supports basic function declarations and return statements.

- Code Generation: The code generator produces x86 assembly code that represents the parsed program. The code generator supports basic integer return statements from the main() function.

# Limitations
- Basic C Syntax: This compiler supports only a very limited subset of C:

- Only simple function declarations with a return statement.
- No support for complex expressions (e.g., arithmetic or logical operations).
- No support for variables, arrays, or functions with parameters.
- No type checking or error handling.
- No Error Handling: The compiler currently does not handle any syntax errors or semantic errors. Invalid input will not produce useful error messages.

- No Optimization: The generated assembly is minimal, with no attempt at optimizing the code.

# Future Improvements
Add support for more complex expressions and operators.
Implement variables, arrays, and function parameters.
Implement error handling for syntax and semantic issues.
Add optimizations to improve the efficiency of generated code.

# Contributing
Feel free to fork the repository, submit issues, and create pull requests for improvements.

