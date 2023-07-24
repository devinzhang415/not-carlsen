#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "util.h"
#include "types.h"


void board_init(const char* fen);
void zobrist_table_init(void);

void make_move(Move move);

bool is_check(bool color);
bool is_attacked(bool color, int square);
bool is_capture(Move move);

bool is_draw();
static bool _is_threefold_rep(void);
static bool _is_fifty_move_rule(void);

uint64_t* get_bitboard(char piece);
uint64_t get_occ_bitboard(bool color);

uint64_t get_attackers(bool color, int square);

void print_board(void);


#endif
