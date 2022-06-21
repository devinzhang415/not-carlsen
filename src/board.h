#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "util.h"


void init(Board* board, Stack** stack, char* fen);
static void _init_zobrist(Board* board);
static void _init_rays(void);
static void _init_zobrist_table(void);
static uint64_t _get_ray(int square1, int square2);

bool legal_push(Board* board, Stack** stack, Move move);
void push(Board* board, Stack** stack, Move move);
void pop(Board* board, Stack** stack);
static void _make_move(Board* board, Move move);

bool is_check(Board* board, bool color);
bool is_attacked(Board* board, bool color, int square);

uint64_t* get_bitboard(Board* board, char piece);


#endif
