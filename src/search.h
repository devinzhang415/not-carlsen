#ifndef SEARCH_H
#define SEARCH_H

#include "util.h"


Result negamax(int depth, int alpha, int beta, bool color, clock_t start, uint64_t* nodes, Move* pv);

Result iterative_deepening();


#endif
