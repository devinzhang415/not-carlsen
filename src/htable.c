#include <stdlib.h>
#include <stdbool.h>
#include "htable.h"
#include "util.h"


extern __thread int* htable;


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
