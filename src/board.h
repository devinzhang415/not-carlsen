#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init(Board* board, Stack** stack, char* fen);

void _make_move(Board* board, Move move);

bool is_check(Board* board, bool color);
bool is_attacked(Board* board, bool color, int square);

uint64_t* get_bitboard(Board* board, char piece);


#endif
