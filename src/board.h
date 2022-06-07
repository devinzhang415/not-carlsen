#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init(Board* board, Stack** stack, char* fen);

void push(Board* board, Stack** stack, Move* move);
void pop(Board* board, Stack** stack);

void _make_move(Board* board, Move* move);
void _update_victim(Board* board, int target);


#endif
