#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdint.h>
#include "util.h"
#include "types.h"


void bishop_attacks_init(void);
void rook_attacks_init(void);
static uint64_t _init_bishop_attacks_helper(int square, uint64_t subset);
static uint64_t _init_rook_attacks_helper(int square, uint64_t subset);
static uint64_t _get_reverse_bb(uint64_t bb);

uint64_t print_divided_perft(int depth);
static uint64_t _perft(int depth);

int gen_legal_moves(Move* moves, bool color);
int gen_legal_captures(Move* moves, bool color);

int get_flag(char piece, int from, int to);

static uint64_t _get_attackmask(bool color);
static uint64_t _get_checkmask(bool color);
static uint64_t _get_pinmask(bool color, int square);

uint64_t get_pawn_moves(bool color, int square);
uint64_t get_knight_moves(bool color, int square);
uint64_t get_bishop_moves(bool color, int square);
uint64_t get_rook_moves(bool color, int square);
uint64_t get_queen_moves(bool color, int square);
uint64_t get_king_moves(bool color, int square);


#endif
