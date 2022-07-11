#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init_board(char* fen);

void make_move(Move move);

bool is_check(bool color);
bool is_attacked(bool color, int square);

bool is_game_over(void);
static bool _is_threefold_rep(void);
static bool _is_fifty_move_rule(void);

uint64_t* get_bitboard(char piece);
uint64_t get_occ_bitboard(bool color);

void print_board(void);


#endif
