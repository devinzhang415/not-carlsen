#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "util.h"
#include "board.h"
#include "rtable.h"


extern Board board;
extern Stack* stack;
extern RTable rtable;


/**
 * Initalizes the stack.
 */
void init_stack() {
    stack = malloc(sizeof(Stack));
    Stack* node = malloc(sizeof(Stack));
    node->move = NULL_MOVE;
    node->board = board;
    node->next = stack;
    stack = node;
}


/**
 * Makes the given move and updates the tables.
 * @param move
 */
void push(Move move) {
    // Update move stack
    Stack* node = malloc(sizeof(Stack));
    _make_move(move);
    node->board = board;
    node->move = move;
    node->next = stack;
    stack = node;

    // Update threefold rep table
    rtable_add(board.zobrist); // TODO rep table causes seg fault at high node counts
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
