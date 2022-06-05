#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init(Board* board, Stack** stack, char* fen);

void push(Stack** stack, Move* move);
void pop(Stack** stack);

void _make_move(Board* board, Move* move);


#endif
