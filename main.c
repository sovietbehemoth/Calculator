#include "include/Calculator.h"

_Bool err = false;



/**
 * @brief Checks for non-whitespace characters.
 * 
 * @param expr Raw expression.
 * @return true There are only whitespace characters.
 * @return false There are non-whitespace characters.
 */
static bool is_empty_expr(char *expr) {
    for (int i = 0; i < strlen(expr); i++) {
        if (expr[i] != ' ' && expr[i] != '\n') { 
            return false;
        }
    } return true;
}




double calculator(char *expr) {
    if (is_empty_expr(expr)) {
        throw("Error: No input.") 0;
    }

    match_t * pstack = lex_parenthesis(expr);             catch return 0;         
    AST_t * ast1 = lexer(expr, pstack, 0);                catch return 0;
    ASTR_t * astr = parser(ast1);                         catch return 0;

    double result = visitor(astr, false);                 catch return 0;
    
    int type = prim_cast(result);

    if (type == VALUE) {
        printf("%d\n", (int) result);
    } else {
        printf("%f\n", result);
    }

    free(pstack);
    free(ast1);
    free(astr);

    return result;
}



static void cmd_processor() {
    while (1) {
        printf("> ");
        
        int size = 10;
        int head = 0;
        
        char *buffer = (char*) calloc(size, sizeof(char));
        
        while (1) {
            int c = fgetc(stdin);
            if (c == EOF || c == '\n') {
                break;
            } else {
                if (head == size) {
                    size += 5;
                    buffer = realloc(buffer, size * sizeof(char));
                }

                buffer[head] = (char) c;
                head++;
            }
        }

        if (head != size && head > 0) {
            buffer = realloc(buffer, head * sizeof(char));
        }

        if (!strcmp(buffer, "exit")) {
            exit(0);
        }

        calculator(buffer); 

        catch {
            err = false;
        }
    }
}



int main() {
    cmd_processor();
    return 0;
}
