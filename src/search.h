#ifndef SEARCH_H
#define SEARCH_H

#include "util.h"


void* iterative_deepening();
Result negamax(int depth, int alpha, int beta, int node_num, bool color, clock_t start, uint64_t* nodes, Move* pv);

int _cmp_moves(const void* elem1, const void* elem2);

static int _score_move(Move move);
static int _get_piece_score(char piece);


#endif
