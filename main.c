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



static char *format_result(double result) {
    int type = prim_cast(result);

    //Should be resized if the decimal precision changes.
    char *buffer = (char*) malloc(20);
    
    if (type == VALUE) {
        sprintf(buffer, "%d", (int) result);
        return buffer;
    } else {
        sprintf(buffer, "%f", result);
    }

    char *ap = (char*) malloc(20);


    bool reached_out = false;

    for (int i = strlen(buffer)-1; i > 0; i--) {
        if (buffer[i] != '0' && !reached_out) {
            reached_out = true;
        }

        if (reached_out) {
            int end = strlen(ap);
            ap[end] = buffer[i];
        }
    }

    char *reverse = (char*) malloc(strlen(ap));

    for (int i = strlen(ap); i >= 0; i--) {
        char d[2];
        d[0] = ap[i];
        d[1] = 0;
        strcat(reverse, d);
    }

    free(ap);
    free(buffer);

    return reverse;
}



double calculator(char *expr) {
    if (is_empty_expr(expr)) {
        throw("Error: No input.") 0;
    }

    match_t * pstack = lex_parenthesis(expr);             catch return 0;         
    AST_t * ast1 = lexer(expr, pstack, 0);                catch return 0;
    ASTR_t * astr = parser(ast1);                         catch return 0;

    double result = visitor(astr, false);                 catch return 0;
    char *fmt = format_result(result);

    puts(fmt);

    free(fmt);
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