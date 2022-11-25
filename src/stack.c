#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "util.h"
#include "board.h"
#include "rtable.h"


extern __thread Board board;
extern __thread Stack stack;


static const uint64_t STACK_INIT_CAPACITY = 1024ULL; // Power of 2 for modulo efficiency


/**
 * Initalizes the stack.
 */
void init_stack() {
    stack.size = 0;
    stack.capacity = STACK_INIT_CAPACITY;
    stack.entries = scalloc(STACK_INIT_CAPACITY, sizeof(Stack_Entry));
    stack.initialized = true;
}


/**
 * Free every element in the stack.
 */
void free_stack(void) {
    if (stack.initialized) {
        free(stack.entries);
        stack.initialized = false;
    }
}


/**
 * Makes the given move and updates the tables.
 * @param move
 */
void push(Move move) {
    // Update move stack
    if (stack.size == stack.capacity) {
        stack.capacity *= 2;
        stack.entries = srealloc(stack.entries, sizeof(Stack_Entry) * stack.capacity);
    }
    Stack_Entry entry = stack.entries[stack.size++];
    make_move(move);
    entry.board = board;
    entry.move = move;
    entry.initialized = true;

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
    stack.entries[stack.size--].initialized = false;
    board = stack.entries[stack.size].board;
}
