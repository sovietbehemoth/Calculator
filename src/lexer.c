#include "../include/Calculator.h"







/**
 * @brief Extracts characters between @start and @end.
 * 
 * @param blob Main buffer.
 * @param start Starting char point.
 * @param end Ending char point.
 * @return char* 
 */
static char *parser_extract_object(char *blob,  int start,  int end) {
    const int len = strlen(blob);
    assert(start <= len);
    assert(end <= len);

    char *cut = (char*) calloc(end - start, sizeof(char));

    int head = 0;

    for (int i = 0; i < end && i < len; i++) {
        if (i > start) {
            cut[head] = blob[i];
            head++;
        }
    }

    return cut;
}



/**
 * @brief Returns pointer to node at @index. 
 * 
 * @param ast Current node or AST.
 * @param index Node number to search for.
 * @param n Distance from top.
 * @return AST_t* 
 */
AST_t *ast_index(AST_t *ast, int index, int n) {
    if (n == index || ast->next == NULL) {
        return ast;
    } else return ast_index(ast->next, index, n+1);
}



/**
 * @brief Turns the raw input into an Abstract Syntax Tree.
 * 
 * @param input Raw input.
 * @param matches Lexed parenthesis stack.
 * @param offset Offset from current position in raw input (used in recursion).
 * @return AST_t* 
 */
AST_t * lexer(char *input, match_t *matches, int offset) {
    catch return NULL;

    if (strlen(input) == 0) {
        throwf("Error[%d]: Empty parenthesized expression.\n", offset) NULL;
    }

    AST_t AST;
    int ast_head = 0;
    
    memset(&AST, 0, AST_DSIZE);


    for (size_t i = 0; i < strlen(input); i++) {
        AST_t* cur = ast_index(&AST, ast_head, 0);

        //This will be an object. This will be evaluated recursively and copied to the object property.
        if (input[i] == '(') {

            int fpair = find_pair(i + offset, matches);
            char *ext = parser_extract_object(input, i, fpair-offset); 

            cur->object = (AST_t*) calloc(1, AST_DSIZE);

            memcpy(cur->object, lexer(ext, matches, offset+1+i), AST_DSIZE);

            i += (strlen(ext)+1);
            
            cur->type = OBJECT;
            cur->next = (AST_t*) calloc(1, AST_DSIZE);
            cur->next->type = UNDEFINED;

            ast_head++;

        //Whitespace is irrelevant; ')' is just the termination of an object so we ignore it entirely.
        } else if (input[i] != ')' && input[i] != ' ') {
            cur->type = CNUMBER;
            cur->c = input[i];

            cur->next = (AST_t*) calloc(1, AST_DSIZE);
            cur->next->type = UNDEFINED;

            ast_head++;
        }
    }

    //Don't return a local variable.
    AST_t * ast_cast = calloc(1, AST_DSIZE);

    memmove(ast_cast, &AST, AST_DSIZE);

    return ast_cast;
}