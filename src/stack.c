#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stack.h"
#include "util.h"
#include "board.h"
#include "rtable.h"

extern _Thread_local Board board;
extern _Thread_local Stack stack;

static const size_t STACK_INIT_CAPACITY = 1 << 8; // Power of 2 for modulo efficiency


/**
 * Initalizes the stack.
 */
void stack_init() {
    stack.size = 0;
    stack.capacity = STACK_INIT_CAPACITY;
    stack.entries = scalloc(STACK_INIT_CAPACITY, sizeof(Stack_Entry));
    _seed();
}


/**
 * Clear the stack.
 */
void stack_clear(void) {
    stack.size = 0;
    memset(stack.entries, 0, stack.capacity * sizeof(Stack_Entry));
    _seed();
}


/**
 * Makes the given move and updates the tables.
 * @param move
 */
void stack_push(Move move) {
    // Update move stack
    if (stack.size == stack.capacity) {
        stack.capacity *= 2;
        stack.entries = srealloc(stack.entries, sizeof(Stack_Entry) * stack.capacity);
    }

    stack.size++;
    make_move(move);
    stack.entries[stack.size].board = board;
    stack.entries[stack.size].move = move;

    rtable_add(board.zobrist);
}


/**
 * @return the move at the top of the stack. 
 */
Move stack_peep(void) {
    return stack.entries[stack.size].move;
}


/**
 * Unmakes the most recent move and updates the tables.
 */
void stack_pop(void) {
    rtable_remove(board.zobrist);
    
    // Update move stack
    stack.size--;
    board = stack.entries[stack.size].board;
}


/**
 * Populate the stack with the initial position.
 */
static void _seed(void) {
    stack.size++;
    stack.entries[stack.size].board = board;
    stack.entries[stack.size].move = NULL_MOVE;

    rtable_add(board.zobrist);
}
