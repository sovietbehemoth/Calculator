#pragma once

//Compare pool operators.
#define CMP_P(n) !strcmp(nstr, n)



//Define if @op is a valid operator.
#define isoperator_c(op) (!!(op == '+' || op == '-' || op == '*' || op == '/' || op == '^' || op == '=' || op == ',' || op == '!' || op == '&' || op == '|'))



//Casts to and from floats.
#define prim_cast(n) (floor(n) == n ? VALUE : VALUE_FLOAT)



//If stack includes such operator. Prevents from executing unnecessary code.
#define HASOP(c) if (ast_hasoperator(*stack, c))



//Assertions and checks to be made before every iteration.
#define DEFASSERTS(c) ASTR_t node = *astr_index(&ast, i, 0); if (i == 0) { pre_visitor_asserts(&ast, #c[0], i); } if (node.type == UNDEFINED) { ast_append(stack, UNDEFINED, 0); break; }



//Main stack allocation for operator concept.
#define SALLOC ASTR_t *stack = (ASTR_t*) calloc(1, ASTR_DSIZE); stack->type = UNDEFINED;



//Catches division by zero.
#define DIVFE(c,v) if (c == '/') if (v == 0) floating_point_exception(); 



//Checks if val is finite and throws if not.
#define FINITE(val) if (!isfinite(val)) {throw("Error: Number evaluated too large (integer overflow).") 0;}



//This is a malleable concept that implements most mathematical operators into the
//general AST data structure. It uses the power of macros for variability.
#define FCMP_OP(op) \
        if (node.next->type == OPERATOR && (char) node.next->value == #op[0]) { \
            visitor_asserts(&ast, i+1, #op[0]); \
            catch return NULL; \
            ASTR_t operand = *astr_index(&ast, i+2, 0); \
            if (node.type == VALUE_FLOAT || node.type == VALUE) { \
                DIVFE(#op[0], operand.value); \
                catch return NULL; \
                double res = node.value op operand.value;\
                FINITE(res); \
                if (!isfinite(res)) { \
                    throw("Error: Number evaluated too large (integer overflow).") 0; \
                } \
                ast_append(stack, prim_cast(res), res); \
            }\
            i += 2;\
        } else {\
            if (node.type == OPERATOR) {\
                ast_append(stack, OPERATOR, (char) node.value);\
            } else if (node.type == VALUE) {\
                ast_append(stack, VALUE, node.value);\
            } else if (node.type == VALUE_FLOAT) {\
                ast_append(stack, VALUE_FLOAT, node.value);\
            }\
        }
