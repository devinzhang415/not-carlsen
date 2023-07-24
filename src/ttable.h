#ifndef TTABLE_H
#define TTABLE_H

#include "util.h"
#include "types.h"


void ttable_init(void);
void ttable_clear(void);

TTable_Entry ttable_get(uint64_t key);
void ttable_add(uint64_t key, int depth, Move move, int score, int flag);


#endif
