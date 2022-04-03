#include "../include/Calculator.h"

//Stores every instance of a parenthesis token for later in the lexer.
typedef struct Token {
    int position;
    int type;
} token_t;

//Stores all pairs of parenthesis.
typedef struct Match {
    int right_position;
    int left_position;
} match_t;



/**
 * @brief Removes two nodes from stack.
 * 
 * @param matches Match stack.
 * @param length Length of match stack.
 * @param target Location of target node.
 * @return token_t* 
 */
static token_t * lexer_remove_popped(token_t *matches, int length, int target) {
    token_t * matches_copy = (token_t*) calloc(length, sizeof(token_t));

    int head = 0;

    for (int i = 0; i < length; i++) {
        if (i == target || i == target+1) {
            continue;
        } 

        //Copy from old stack to new stack.
        memcpy(&matches_copy[head], &matches[i], sizeof(token_t)); 
        head++;
    }

    return matches_copy;
}






/**
 * @brief Lexically pairs all matching parenthesis and curates into a collection.
 * 
 * @param input Raw input.
 * @return match_t* 
 */
match_t * lex_parenthesis(char *input) {
    int stack_size = 10;
    int stack_head = 0;    

    int lcount = 0;
    int rcount = 0;

    token_t * stack = (token_t*) calloc(stack_size, sizeof(token_t));

    //The first stack just finds every instance of parenthesis.

    for (size_t i = 0; i < strlen(input); i++) {
        if (input[i] == '(' || input[i] == ')') {
            if (stack_head == stack_size) {
                stack_size += 5;
                stack = realloc(stack, stack_size * sizeof(token_t));
            }

            stack[stack_head].position = i;

            if (input[i] == '(') {
                stack[stack_head].type = LEFT;
                lcount++;
            } else {
                stack[stack_head].type = RIGHT;
                rcount++;
            }

            stack_head++;
        }
    }

    //Use less memory by trimming block.

    if (stack_size != stack_head) {
        int rsize = rcount + lcount;
        stack = realloc(stack, rsize * sizeof(token_t));
    }

    if (lcount != rcount) {
        free(stack);

        if (lcount < rcount) {
            throw("Syntax, ')' instances expect '('.\n") NULL;
        } else {
            throw("Syntax, '(' instance expect ')'.\n") NULL;
        }
    }

    //The pairs will be created by brute force finding each pair and removing it
    //afterward. This could theoretically get stuck upon lexing error.

    int iterator = 0;
    int matches_head = 0;
    int stack_head_cpy = stack_head;
    int matches_size = 5;

    int iterations = 0;

    match_t * matches = (match_t*) calloc(matches_size, sizeof(match_t));

    while (stack_head_cpy > 0) {

        if (iterator >= stack_head_cpy) {
            iterator = 0; iterations++;
            continue;
        }

        if (iterator + 1 <= stack_head) {
            
            if (stack[iterator].type == LEFT && stack[iterator+1].type == RIGHT) {
                if (matches_size == matches_head) {
                    matches_size += 5;
                    matches = realloc(matches, matches_size * sizeof(match_t));
                }

                matches[matches_head].left_position = stack[iterator].position;
                matches[matches_head].right_position = stack[iterator+1].position;

                //Remove leading tokens.
                stack = lexer_remove_popped(stack, stack_head, iterator);

                stack_head_cpy -= 2;
                matches_head++;
            }
        }

        iterator++; iterations++;
    }


    int terminator = (matches_head == 0 ? 0 : matches_head + 1);

    if (matches_head != matches_size) {
        if (matches_head == 0) {
            matches_head = 8; //In case no parenthesis are present in the expression.
        } else {
            matches = realloc(matches, matches_head * sizeof(match_t));
        }
    }

    

    //Last member acts as a terminator for the length checker.
    matches[terminator].left_position = -1;
    matches[terminator].right_position = -1;

    free(stack);

    return matches;
}





/**
 * @brief Measure length of match stack by finding terminator. Crucial for safety.
 * 
 * @param lex Match stack.
 * @return int 
 */
static int lex_length_check_p(match_t *lex) {
    for (int i = 0;; i++) {
        if (lex[i].left_position == -1) {
            return i;
        }
    }
}



/**
 * @brief Searches match stack for the pair of left parenthesis.
 * 
 * @param index Index.
 * @param matches Match stack.
 * @return int 
 */
unsigned int find_pair(int index, match_t *matches) {
    int length = lex_length_check_p(matches);

    for (int i = 0; i < length; i++) {
        if (matches[i].left_position == index) {
            return matches[i].right_position;
        }
    }

    return 0;
}