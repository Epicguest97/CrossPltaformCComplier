#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Simplified token types
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_INTEGER,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATION,
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

// Lexer structure
typedef struct {
    char* source;
    int position;
    int line;
    int column;
} Lexer;

// Abstract Syntax Tree (AST) node types
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DECLARATION,
    AST_RETURN_STATEMENT,
    AST_INTEGER_LITERAL
} ASTNodeType;

// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        char* identifier;
        int integer_value;
        struct {
            struct ASTNode* body;
            char* name;
        } function_decl;
        struct {
            struct ASTNode* return_value;
        } return_stmt;
    } data;
    struct ASTNode* next;
} ASTNode;

// Function prototypes
Lexer* create_lexer(char* source);
void free_lexer(Lexer* lexer);
Token* tokenize(Lexer* lexer);
ASTNode* parse(Token* tokens);
void generate_code(ASTNode* ast, FILE* output);
void free_ast(ASTNode* node);

// Create a new lexer
Lexer* create_lexer(char* source) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->source = strdup(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    return lexer;
}

// Free lexer memory
void free_lexer(Lexer* lexer) {
    free(lexer->source);
    free(lexer);
}

// Check if a character is a valid identifier start
int is_identifier_start(char c) {
    return isalpha(c) || c == '_';
}

// Check if a character is a valid identifier continuation
int is_identifier_char(char c) {
    return isalnum(c) || c == '_';
}

// Tokenization function
Token* tokenize(Lexer* lexer) {
    // In a real compiler, this would be dynamically allocated
    static Token tokens[1000];
    int token_count = 0;

    while (lexer->source[lexer->position] != '\0') {
        char current = lexer->source[lexer->position];

        // Skip whitespace
        while (isspace(current)) {
            if (current == '\n') {
                lexer->line++;
                lexer->column = 1;
            } else {
                lexer->column++;
            }
            lexer->position++;
            current = lexer->source[lexer->position];
        }

        // Identifiers and keywords
        if (is_identifier_start(current)) {
            int start = lexer->position;
            while (is_identifier_char(lexer->source[lexer->position])) {
                lexer->position++;
            }
            
            int length = lexer->position - start;
            char* value = malloc(length + 1);
            strncpy(value, &lexer->source[start], length);
            value[length] = '\0';

            // Check if it's a keyword
            TokenType type = strcmp(value, "int") == 0 || 
                             strcmp(value, "return") == 0 ? 
                             TOKEN_KEYWORD : TOKEN_IDENTIFIER;

            tokens[token_count].type = type;
            tokens[token_count].value = value;
            tokens[token_count].line = lexer->line;
            tokens[token_count].column = lexer->column;
            token_count++;
            
            continue;
        }

        // Integer literals
        if (isdigit(current)) {
            int start = lexer->position;
            while (isdigit(lexer->source[lexer->position])) {
                lexer->position++;
            }
            
            int length = lexer->position - start;
            char* value = malloc(length + 1);
            strncpy(value, &lexer->source[start], length);
            value[length] = '\0';

            tokens[token_count].type = TOKEN_INTEGER;
            tokens[token_count].value = value;
            tokens[token_count].line = lexer->line;
            tokens[token_count].column = lexer->column;
            token_count++;
            
            continue;
        }

        // Punctuation
        if (current == '(' || current == ')' || current == '{' || 
            current == '}' || current == ';') {
            char* value = malloc(2);
            value[0] = current;
            value[1] = '\0';

            tokens[token_count].type = TOKEN_PUNCTUATION;
            tokens[token_count].value = value;
            tokens[token_count].line = lexer->line;
            tokens[token_count].column = lexer->column;
            token_count++;

            lexer->position++;
            lexer->column++;
            continue;
        }

        // If no match, error
        printf("Unexpected character: %c at line %d, column %d\n", 
               current, lexer->line, lexer->column);
        lexer->position++;
        lexer->column++;
    }

    // End of file token
    tokens[token_count].type = TOKEN_EOF;
    tokens[token_count].value = NULL;
    tokens[token_count].line = lexer->line;
    tokens[token_count].column = lexer->column;

    return tokens;
}

// Simple recursive descent parser
ASTNode* parse(Token* tokens) {
    ASTNode* program = malloc(sizeof(ASTNode));
    program->type = AST_PROGRAM;
    program->next = NULL;

    int i = 0;
    // Look for function declaration
    while (tokens[i].type != TOKEN_EOF) {
        // Check for 'int' keyword
        if (tokens[i].type == TOKEN_KEYWORD && 
            strcmp(tokens[i].value, "int") == 0) {
            i++;

            // Check for function name
            if (tokens[i].type == TOKEN_IDENTIFIER) {
                ASTNode* function = malloc(sizeof(ASTNode));
                function->type = AST_FUNCTION_DECLARATION;
                function->data.function_decl.name = strdup(tokens[i].value);
                i++;

                // Check for opening parenthesis
                if (tokens[i].type == TOKEN_PUNCTUATION && 
                    strcmp(tokens[i].value, "(") == 0) {
                    i++;
                    // Close parenthesis
                    if (tokens[i].type == TOKEN_PUNCTUATION && 
                        strcmp(tokens[i].value, ")") == 0) {
                        i++;

                        // Open brace
                        if (tokens[i].type == TOKEN_PUNCTUATION && 
                            strcmp(tokens[i].value, "{") == 0) {
                            i++;

                            // Look for return statement
                            if (tokens[i].type == TOKEN_KEYWORD && 
                                strcmp(tokens[i].value, "return") == 0) {
                                i++;

                                // Return value must be an integer
                                if (tokens[i].type == TOKEN_INTEGER) {
                                    ASTNode* return_stmt = malloc(sizeof(ASTNode));
                                    return_stmt->type = AST_RETURN_STATEMENT;
                                    
                                    ASTNode* literal = malloc(sizeof(ASTNode));
                                    literal->type = AST_INTEGER_LITERAL;
                                    literal->data.integer_value = atoi(tokens[i].value);
                                    
                                    return_stmt->data.return_stmt.return_value = literal;
                                    function->data.function_decl.body = return_stmt;
                                    i++;
                                }
                            }

                            // Close brace
                            if (tokens[i].type == TOKEN_PUNCTUATION && 
                                strcmp(tokens[i].value, "}") == 0) {
                                // Add function to program
                                function->next = program->next;
                                program->next = function;
                            }
                        }
                    }
                }
            }
        }
        i++;
    }

    return program;
}

// Code generation - output x86 assembly
void generate_code(ASTNode* ast, FILE* output) {
    fprintf(output, ".global main\n");
    fprintf(output, "main:\n");

    // Traverse AST and generate code
    ASTNode* current = ast->next;
    while (current != NULL) {
        if (current->type == AST_FUNCTION_DECLARATION) {
            ASTNode* body = current->data.function_decl.body;
            if (body && body->type == AST_RETURN_STATEMENT) {
                ASTNode* return_value = body->data.return_stmt.return_value;
                if (return_value->type == AST_INTEGER_LITERAL) {
                    // x86 assembly for returning a value
                    fprintf(output, "    movl $%d, %%eax\n", return_value->data.integer_value);
                    fprintf(output, "    ret\n");
                }
            }
        }
        current = current->next;
    }
}

// Free AST memory
void free_ast(ASTNode* node) {
    while (node != NULL) {
        ASTNode* next = node->next;

        // Free based on node type
        switch (node->type) {
            case AST_FUNCTION_DECLARATION:
                free(node->data.function_decl.name);
                // Free body nodes recursively
                if (node->data.function_decl.body) {
                    free_ast(node->data.function_decl.body);
                }
                break;
            case AST_RETURN_STATEMENT:
                if (node->data.return_stmt.return_value) {
                    free(node->data.return_stmt.return_value);
                }
                break;
            default:
                break;
        }

        free(node);
        node = next;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Read input file
    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        printf("Could not open input file\n");
        return 1;
    }

    // Get file size
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    rewind(input_file);

    // Read file content
    char* source_code = malloc(file_size + 1);
    fread(source_code, 1, file_size, input_file);
    source_code[file_size] = '\0';
    fclose(input_file);

    // Create lexer
    Lexer* lexer = create_lexer(source_code);

    // Tokenize
    Token* tokens = tokenize(lexer);

    // Parse
    ASTNode* ast = parse(tokens);

    // Open output file for assembly
    FILE* output_file = fopen(argv[2], "w");
    if (!output_file) {
        printf("Could not open output file\n");
        return 1;
    }

    // Generate code
    generate_code(ast, output_file);

    // Clean up
    fclose(output_file);
    free_ast(ast);
    free_lexer(lexer);
    free(source_code);

    printf("Compilation successful\n");
    return 0;
}