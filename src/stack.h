#ifndef STACK_H
#define STACK_H

#include "util.h"


void init_stack(Board* board, Stack** stack);

bool legal_push(Board* board, Stack** stack, Move move);
void push(Board* board, Stack** stack, Move move);
void pop(Board* board, Stack** stack);

#endif
