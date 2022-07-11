#ifndef TTABLE_H
#define TTABLE_H

#include "util.h"


extern const uint64_t TTABLE_INIT_CAPACITY;


void init_ttable(void);

TTable_Entry ttable_get(uint64_t key);
void ttable_add(uint64_t key, int depth, Move move, int score, int flag);


#endif
