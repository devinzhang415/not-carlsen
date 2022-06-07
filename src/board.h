#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init(Board* board, Stack** stack, char* fen);

bool push_if_legal(Board* board, Stack** stack, Move* move);

void push(Board* board, Stack** stack, Move* move);
void pop(Board* board, Stack** stack);
void _make_move(Board* board, Move* move);

bool is_check(Board* board, bool color);
bool is_attacked(Board* board, bool color, int square);


#endif
