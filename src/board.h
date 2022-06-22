#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init(Board* board, Stack** stack, RTable* rtable, char* fen);

void _make_move(Board* board, Move move);

bool is_check(Board* board, bool color);
bool is_attacked(Board* board, bool color, int square);

bool is_game_over(Board* board, RTable* rtable);
static bool _is_threefold_rep(Board* board, RTable* rtable);
static bool _is_fifty_move_rule(Board* board);

uint64_t* get_bitboard(Board* board, char piece);

void print_mailbox(char* mailbox);


#endif
