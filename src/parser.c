#include "../include/Calculator.h"



/**
 * @brief Returns pointer to node at @index.
 * 
 * @param ast 
 * @param index 
 * @param n 
 * @return ASTR_t* 
 */
ASTR_t *astr_index(ASTR_t *ast, int index, int n) {
    if (n == index || ast->next == NULL) {
        return ast;
    } else return astr_index(ast->next, index, n+1);
}



/**
 * @brief Determine if char is valid number.
 * 
 * @param n Char.
 * @return bool
 */
static bool isnumber(char n) {
    if (n == '1' || n == '2' || n == '3' || n == '4' || n == '5' || n == '6' || n == '7' || n == '8' || n == '9' || n == '0' || n == '.') {
        return true;
    } return false;
}



/**
 * @brief If @str contains @c. Just a basic helper function.
 * 
 * @param str String.
 * @param c Char.
 * @return true 
 * @return false 
 */
static bool contains(char *str, char c) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == c) {
            return true;
        }
    } return false;
}



/**
 * @brief Pushes a node to AST and shifts terminator. Supports all primitives.
 * 
 * @param ast Abstract syntax tree.
 * @param type Type of node.
 * @param value Raw value.
 * @return int 
 */
int ast_append(ASTR_t *ast, int type, double value) {

    int pos;

    for (int i = 0 ;; i++) {    //Find length of AST.
        ASTR_t head = *astr_index(ast, i, 0);

        if (head.type == UNDEFINED) {
            pos = i;
            break;
        }
    }
    
    ASTR_t *node = astr_index(ast, pos, 0);
 
    node->type = type;

    if (type == VALUE_FLOAT) {
        node->value = value;
    } else node->value = type == OPERATOR ? (char) value : value;
    

    node->next = (ASTR_t*) calloc(1, ASTR_DSIZE);
    node->next->type = UNDEFINED; //Shift the terminator (critical for safety).

    return pos;
}



/**
 * @brief Performs several tasks to further organize the AST.
 * 
 * @param ast Abstract syntax tree.
 * @return AST_t* 
 */
ASTR_t *parser(AST_t *ast) {

    int astr_head = 0;
    ASTR_t astr;

    memset(&astr, 0, ASTR_DSIZE);

    //Very important to terminate the AST.
    astr.type = UNDEFINED;  

    for (int i = 0 ;; i++) {

        AST_t node = *ast_index(ast, i, 0);
        
        if (node.type == UNDEFINED) {
            break;
        }

        //Object handling requires recursion.
        if (node.type == OBJECT) {
            ASTR_t *ap = astr_index(&astr, astr_head+1, 0);

            ap->type = OBJECT;
            ap->object = (ASTR_t*) calloc(1, ASTR_DSIZE);
            ap->next = (ASTR_t*) calloc(1, ASTR_DSIZE);
            ap->next->type = UNDEFINED;

            memcpy(ap->object, parser(node.object), ASTR_DSIZE);

            astr_head++;

        } else {

            //The pool is just a collection of characters read into input
            //that the parser must now make sense of.

            int pool_size = 10;
            int pool_head = 0;
            char *pool = (char*) calloc(pool_size, sizeof(char));

            //Exemption enables special handling for numbers with peculiar cases.
            bool exemption = false;

            //Exponential notation.
            if (i - 1 > 0) {
                AST_t prev = *ast_index(ast, i-1, 0);
                if (prev.type == CNUMBER && node.c == 'e' && !isoperator_c(prev.c)) {
                    exemption = true;
                }
            } 

            if (isnumber(node.c) || exemption) {
                int j;
                bool isop = false;
                char op;

                for (j = i ;; j++) {
                    AST_t subnode = *ast_index(ast, j, 0);

                    if (subnode.type == UNDEFINED) {
                        break;
                    } 

                    //Expected chars.
                    if ( (isnumber(subnode.c) || subnode.c == '.' || subnode.c == 'e') && (!isoperator_c(subnode.c)) && subnode.type != OBJECT) {
                        AST_t lookahead = *ast_index(ast, j+1, 0);


                        if (subnode.c == 'e' && (lookahead.c == '+' || lookahead.c == '-') ) {
                            if (pool_head == pool_size || pool_head + 2 >= pool_size) {
                                pool_size += 5;
                                pool = realloc(pool, sizeof(char) * pool_size);
                            }

                            if (lookahead.c == '+') {
                                strcat(pool, "e+");
                            } else strcat(pool, "e-");

                            pool_head += 2;
                        } else {

                            if (pool_head == pool_size || pool_head + 2 >= pool_size) {
                                pool_size += 5;
                                pool = realloc(pool, sizeof(char) * pool_size);
                            }
                            pool[pool_head] = subnode.c;
                            pool_head++;
                        }

                        if (exemption) {
                            exemption = false;
                        }
                    } else if (isoperator_c(subnode.c)) {
                        isop = true;
                        op = subnode.c;
                        break;
                    } else {
                        j--; //We've found something that should be handled by the next iteration.
                        break;
                    }

                }

                i = j; 

                if (!strcmp(pool, "e")) {
                    ast_append(&astr, VALUE, M_E);
                    astr_head++;
                } else {
                    //Cast to correct type.
                    if (contains(pool, '.')) {
                        ast_append(&astr, VALUE_FLOAT, atof(pool));
                    } else ast_append(&astr, VALUE, atof(pool));

                    astr_head++;
                }

                if (isop) { 
                    //Read operator.
                    isop = false;
                    ast_append(&astr, OPERATOR, op);
                }

                continue;
            } else { //Represent any extraneous values.

                int nstr_head = 0;
                int nstr_size = 5;

                char *nstr = (char*) calloc(nstr_size, sizeof(char));

                int j;
                bool isop = false;
                char op;

                for (j = i ;; j++) {
                    AST_t subnode = *ast_index(ast, j, 0);

                    if (subnode.type == UNDEFINED) {
                        break;
                    } 

                    if (!isnumber(subnode.c) && subnode.type == CNUMBER && !isoperator_c(subnode.c)) {
                        if (nstr_head == nstr_size) {
                            nstr_size += 5;
                            nstr = realloc(nstr, nstr_size * sizeof(char));
                        } nstr[nstr_head] = subnode.c;
                        nstr_head++;
                    } else if (isoperator_c(subnode.c)) {
                        isop = true;
                        op = subnode.c;
                        break;
                    } else break;
                }

                i = j;

                
                //These values represent identifiers that will be entered in the calculator,
                //but represent integer values, thus the parser must evaluate them as such.

                if (strlen(nstr) > 0) {
                    if (CMP_P("pi")) {
                        ast_append(&astr, VALUE_FLOAT, M_PI);
                    } else if (CMP_P("e")) {
                        ast_append(&astr, VALUE_FLOAT, M_E);
                    } else if (CMP_P("false")) {
                        ast_append(&astr, VALUE, 0);
                    } else if (CMP_P("true")) {
                        ast_append(&astr, VALUE, 1);
                    } else { 
                        throw("Error: Unidentified macro or identifier.") NULL;

                        //Else means a value that is a string value, but not identified as a macro.
                        //We preserve this value for function calls. It may be more useful to evaluate
                        //if it is a valid function here, but not for now, since this is the parser.

                        //ast_append() only supports integer types for appending, we must implement ast_append()
                        //for strings here.
            
                        int pos;

                        for (int i = 0 ;; i++) {    //Find length of AST.
                            ASTR_t head = *astr_index(&astr, i, 0);

                            if (head.type == UNDEFINED) {
                                pos = i;
                                break;
                            }
                        }
                        
                        ASTR_t *node = astr_index(&astr, pos, 0);

                        node->type = IDENTIFIER;
                        node->id = (char*) calloc(strlen(nstr), sizeof(char));
                        node->next = (ASTR_t*) calloc(1, AST_DSIZE);
                        node->next->type = UNDEFINED; //Shift the terminator (critical for safety).

                        //strcpy() could also be used here.
                        memcpy(node->id, nstr, strlen(nstr) * sizeof(char));
                    }
                }

                if (isop) { //operators are pushed here.
                    ast_append(&astr, OPERATOR, op);
                    // dbprintf("CHAR: %c\n", (char) astr_index(&astr, q, 0)->value);
                }

                if (isnumber(node.c)) {
                    i--;
                }
                
                continue;
            }
        }
    }

    //Don't return a local variable.
    ASTR_t * ast_cast = calloc(1, ASTR_DSIZE);
    
    memmove(ast_cast, &astr, ASTR_DSIZE);
    
    return ast_cast;
}

void debug_astr(ASTR_t * ast, int type) {
    for (int i = 0 ;; i++) {
        ASTR_t cur = *astr_index(ast, i, 0);

        if (cur.type == OPERATOR) {
            if (type == 0) putchar((char) cur.value);
            else printf("Operator: %c\n", (char) cur.value);
        } else if (cur.type == OBJECT) {
            if (type == 0) {
                putchar('(');
                debug_astr(cur.object, type);
                putchar(')');
            } else {
                puts("~~~START OBJECT~~~");
                debug_astr(cur.object, type);
                puts("~~~END OBJECT~~~");
            }
        } else if (cur.type == IDENTIFIER) {
            if (type == 0) printf("%s", cur.id);
            else printf("Identifier: %s\n",cur.id);
        } else if (cur.type == VALUE) {
            if (type == 0) printf("%d", (int) cur.value);
            else printf("Number: %d\n", (int) cur.value);
        } else if (cur.type == VALUE_FLOAT) { 
            if (type == 0) printf("%f", cur.value);
            else printf("Number: %f\n", cur.value);
        } else {
            return;
        }
    }
}