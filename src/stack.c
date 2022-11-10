#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "util.h"
#include "board.h"
#include "rtable.h"


extern __thread Board board;
extern __thread Stack* stack;


static bool initialized = false;


/**
 * Initalizes the stack.
 */
void init_stack() {
    free_stack();
    stack = smalloc(sizeof(Stack));
    stack->move = NULL_MOVE;
    stack->board = board;
    stack->next = NULL;
    initialized = true;
}


/**
 * Free every element in the stack.
 */
void free_stack(void) {
    if (!initialized) return;
    while (stack != NULL) {
        Stack* temp = stack;
        stack = stack->next;
        free(temp);
    }
    initialized = false;
}


/**
 * Makes the given move and updates the tables.
 * @param move
 */
void push(Move move) {
    // Update move stack
    Stack* node = smalloc(sizeof(Stack));
    make_move(move);
    node->board = board;
    node->move = move;
    node->next = stack;
    stack = node;

    // Update threefold rep table
    rtable_add(board.zobrist);
}


/**
 * Unmakes the most recent move and updates the tables.
 */
void pop(void) {
    // Update threefold rep table
    rtable_remove(board.zobrist);
    
    // Update move stack
    Stack* temp = stack;
    stack = stack->next;
    board = stack->board;
    free(temp);
}
