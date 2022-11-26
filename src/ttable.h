#ifndef TTABLE_H
#define TTABLE_H

#include "util.h"


void init_ttable(void);
void clear_ttable(void);

TTable_Entry ttable_get(uint64_t key);
void ttable_add(uint64_t key, int depth, Move move, int score, int flag);


#endif
