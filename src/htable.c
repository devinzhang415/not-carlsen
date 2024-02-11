#include <stdlib.h>
#include <stdbool.h>
#include "htable.h"
#include "util.h"


extern int* htable;

static const size_t HTABLE_CAPACITY = 2 * 64 * 64; // sides * squares from * squares to


/**
 * Initalizes the history heuristic table.
 */
void htable_init(void) {
    htable = scalloc(HTABLE_CAPACITY, sizeof(int));
}


/**
 * Clears the history heuristic table.
 */
void htable_clear(void) {
    memset(htable, 0, HTABLE_CAPACITY * sizeof(int));
}


/**
 * @param color the side to move.
 * @param from the square the move is from.
 * @param to the square the move is to.
 * @return a heuristic value based on how many times this move has been searched.
 */
int htable_get(int color, int from, int to) {
    return *(htable + color*64*64 + from*64 + to);
}


/**
 * Increments the history heuristic value for a given move.
 * 
 * @param color the side to move.
 * @param from the square the move is from.
 * @param to the square the move is to.
 * @param depth the depth this move was played at.
 */
void htable_add(int color, int from, int to, int depth) {
    *(htable + color*64*64 + from*64 + to) += depth * depth;
}
