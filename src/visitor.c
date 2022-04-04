#include "../include/Calculator.h"


/**
 * @brief Trigger a floating point exception (divide by zero in this use case).
 * 
 * @return _No_return_ Never.
 */
void floating_point_exception() {
    throw("Error: Divide by zero.");
} 



/**
 * @brief Measure length of second AST iteration.
 * 
 * @param ast 
 * @param depth 
 * @return int 
 */
static int astr_length(ASTR_t *ast, int depth) {
    if (ast->type == UNDEFINED) {
        return depth;
    } else return astr_length(ast->next, depth+1);
}



static ASTR_t *ast_implicit_mult(ASTR_t *stack) {
    ASTR_t *copy = (ASTR_t*) calloc(1, ASTR_DSIZE);
        
    copy->type = UNDEFINED;

    bool defined = false;
    double eval;

    for (int i = 0 ;; i++) {
        ASTR_t node = *astr_index(stack, i, 0);

        if (node.type == VALUE || node.type == VALUE_FLOAT) {
            if (node.next->type == VALUE || node.next->type == VALUE_FLOAT) {
                if (!defined) {
                    defined = true;
                    eval = node.value * node.next->value;
                    i++;
                    continue;
                } 
            } 

            eval *= node.value;
        }
            
        if (node.type == UNDEFINED) {
            ast_append(copy, prim_cast(eval), eval);
            ast_append(copy, UNDEFINED, -1);
            break;
        }
    }

    if (!defined) {
        err = true;
        return NULL;
    }

    return copy;
}



/**
 * @brief Makes sure that AST includes specified operator. Saves unnecesary code execution.
 * 
 * @param ast 
 * @param operator 
 * @return true 
 * @return false 
 */
static bool ast_hasoperator(ASTR_t ast, char operator) {
    for (int i = 0 ;; i++) {
        ASTR_t node = *astr_index(&ast, i, 0);

        if (node.type == UNDEFINED) {
            return false;
        }
 
        if (node.type == OPERATOR) {
            if ((char) node.value == operator) {
                return true;
            }
        }
    } return false;
}



/**
 * @brief Checks for bad syntax.
 * 
 * @param ast 
 * @param index 
 * @param operator 
 */
static void visitor_asserts(ASTR_t *ast, int index, char operator) {
    ASTR_t node = *astr_index(ast, index, 0);
    ASTR_t prev = *astr_index(ast, index - 1, 0);

    if (node.next->type == UNDEFINED) {
        throwf("Error: Operator '%c' expects right operand.\n", (char) node.value);
    }

    if (isoperator_c((char)prev.value) || isoperator_c((char)node.next->value)) {
        char bad_operator = isoperator_c((char)prev.value) ? (char) prev.value : (char) node.next->value;

        throwf("Error: Unexpected operator '%c'.\n", bad_operator);
    }
}

/**
 * @brief Checks for incomplete expressions.
 * 
 * @param ast AST.
 * @param index Position in tree.
 */
static void pre_visitor_asserts(ASTR_t *ast, char operator, int index) {
    ASTR_t node = *astr_index(ast, index, 0);

    if (isoperator_c((char) node.value)) {
        throwf("Error: Operator '%c' expects left operand.\n", (char) node.value);
    }
}

/**
 * @brief Evaluates all objects.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_parenthesize(ASTR_t ast) {
    ASTR_t *paren = (ASTR_t*) calloc(1, ASTR_DSIZE);

    paren->type = UNDEFINED;

    for (int i = 0 ;; i++) {
        ASTR_t node = *astr_index(&ast, i, 0);
        
        bool breakl = false; 

        switch (node.type) {
            case UNDEFINED:
                ast_append(paren, UNDEFINED, 0);
                breakl = true;
                break;
            case OBJECT:
                //Recursion for nesting.
                ;double eval = visitor(node.object, true);
                ast_append( paren, node.object->type, eval);
                break;
            case VALUE:
                if (node.next->type == VALUE || node.next->type == VALUE_FLOAT) {
                    double op = node.value * node.next->value;
                    ast_append( paren, prim_cast(op),  op);
                } else ast_append( paren, VALUE, node.value );
                break;
            case VALUE_FLOAT:
                if (node.next->type == VALUE) {
                    double op = node.value * node.next->value;
                    ast_append( paren, prim_cast(op), op);
                } else ast_append( paren, VALUE_FLOAT, node.value );
                break;
            case OPERATOR:
                ast_append(paren, OPERATOR, (char) node.value);
                break;
        } 

        if (breakl) {
            break;
        }
    }

    return paren;
}

/**
 * @brief Signs all integers that should be signed.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_sign(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        ASTR_t node = *astr_index(&ast, i, 0); 
        
        if (node.type == UNDEFINED) { 
            ast_append(stack, UNDEFINED, 0); 
            break; 
        }

        ASTR_t prev = *astr_index(&ast, i-1 >= 0 ? i-1 : 0, 0);
        int type = node.next->type;

        //The previous value MUST be an operator, or else it is a subtraction operation.
        if ((prev.type == OPERATOR || i == 0) && (char) node.value == '-') {
            if (node.next->value == 0) {
                throw( "Error: Cannot sign zero.") NULL;
            }
            
            if (node.next->type != VALUE && node.next->type != VALUE_FLOAT) {
                throwf("Error: Cannot sign operator '%c'.", (char) node.next->value) NULL;
            }

            ast_append(stack, type, -(node.next->value));
            i++;
        } else {
            if (node.type == OPERATOR && (char) node.value == '-' && type == UNDEFINED) {
                throw("Error: Unexpected operator at end of expression.") NULL;
            }

            if (prev.type == VALUE_FLOAT) {
                ast_append(stack, VALUE_FLOAT, node.value);
            } else ast_append(stack, node.type, node.value);
        }
    }
    
    return stack;
}

/**
 * @brief Raises powers. Is not fully macro-ized because it uses a function call to evaluate.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_raise(ASTR_t ast) {

    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(^);

        if (node.next->type == OPERATOR && (char) node.next->value == '^') {
            visitor_asserts(&ast, i+1, '^');

            ASTR_t operand = *astr_index(&ast, i+2, 0);

            if (node.type == VALUE_FLOAT) {
                double res = pow(node.value, operand.value);
                FINITE(res);
                ast_append(stack, prim_cast(res), res);
            } else if (node.type == VALUE) {
                double res = pow(node.value, operand.value);
                FINITE(res);
                ast_append(stack, prim_cast(res), res);
            }
            
            i += 2;
        } else {
            if (node.type == OPERATOR) {
                ast_append(stack, OPERATOR, (char) node.value);
            } else if (node.type == VALUE) {
                ast_append(stack, VALUE, node.value);
            } else if (node.type == VALUE_FLOAT) {
                ast_append(stack, VALUE_FLOAT, node.value);
            }
        }
    }

    return stack;
}

/**
 * @brief Performs unary operations.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_unary(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(*);
        FCMP_OP(*);
    }
    
    return stack;
}

/**
 * @brief Performs division operations.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_div(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(/);
        FCMP_OP(/);
    }
    
    return stack;
}

/**
 * @brief Performs addition operations.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_add(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(+);
        FCMP_OP(+);
    }
    
    return stack;
}

/**
 * @brief Performs subtraction operations.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_sub(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(-);
        FCMP_OP(-);
    }
    
    return stack;
}

/**
 * @brief Performs equality boolean evaluation.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_eq(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(=);
        FCMP_OP(==);
    }
    
    return stack;
}

/**
 * @brief Performs inverted equality boolean evaluation.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_neq(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(!);
        FCMP_OP(!=);
    }
    
    return stack;
}

/**
 * @brief Performs boolean AND logic.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_andeq(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(&);
        FCMP_OP(&&);
    }
    
    return stack;
}

/**
 * @brief Performs boolean OR logic.
 * 
 * @param ast 
 * @return ASTR_t* 
 */
static ASTR_t *visitor_oreq(ASTR_t ast) {
    SALLOC

    for (int i = 0 ;; i++) {
        DEFASSERTS(|);
        FCMP_OP(||);
    }
    
    return stack;
}



double visitor(ASTR_t *ast, bool recur) {
    catch return 0;

    ASTR_t *stack = (ASTR_t*) calloc(1, ASTR_DSIZE);

    //Used by memcpy() alias.
    ASTR_t *sw;

    memcpy(stack, ast, ASTR_DSIZE);    
    _memcpy(stack, visitor_parenthesize(*stack), ASTR_DSIZE);        catch return 0;

    HASOP('-') _memcpy(stack, visitor_sign(*stack), ASTR_DSIZE);     catch return 0;
    HASOP('^') _memcpy(stack, visitor_raise(*stack), ASTR_DSIZE);    catch return 0;
    HASOP('*') _memcpy(stack, visitor_unary(*stack), ASTR_DSIZE);    catch return 0;
    HASOP('/') _memcpy(stack, visitor_div(*stack), ASTR_DSIZE);      catch return 0;
    HASOP('+') _memcpy(stack, visitor_add(*stack), ASTR_DSIZE);      catch return 0;
    HASOP('-') _memcpy(stack, visitor_sub(*stack), ASTR_DSIZE);      catch return 0;
    HASOP('=') _memcpy(stack, visitor_eq(*stack), ASTR_DSIZE);       catch return 0;
    HASOP('!') _memcpy(stack, visitor_neq(*stack), ASTR_DSIZE);      catch return 0;
    HASOP('&') _memcpy(stack, visitor_andeq(*stack), ASTR_DSIZE);    catch return 0;
    HASOP('|') _memcpy(stack, visitor_oreq(*stack), ASTR_DSIZE);     catch return 0;


    int len = astr_length(stack, 0);

    if (len > 1 || stack->type == OPERATOR) {

        if (len == 1) {
            throwf("Error: unexpected use of '%c' operator\n", (char) stack->value) 0;
        }

        if (ast_hasoperator(*stack, ',')) {
            throw("Error: Improper use of comma operator.") 0;
        }
        
        ASTR_t *isolate = ast_implicit_mult(stack);

        catch {
            int n = visitor(stack, true);
            err = false;
            return n;
        } else return isolate->value;
    }

    return stack->value;
}



