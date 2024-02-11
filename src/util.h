#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "types.h"


extern const bool WHITE;
extern const bool BLACK;

extern const int SQUARE_NUM;
extern const int MAX_PIECE_NUM;

extern const uint64_t BB_SQUARES[64];

extern const uint64_t BB_ALL;

extern const uint64_t BB_LIGHT_SQUARES;
extern const uint64_t BB_DARK_SQUARES;

extern const uint64_t BB_FILE_A;
extern const uint64_t BB_FILE_B;
extern const uint64_t BB_FILE_C;
extern const uint64_t BB_FILE_D;
extern const uint64_t BB_FILE_E;
extern const uint64_t BB_FILE_F;
extern const uint64_t BB_FILE_G;
extern const uint64_t BB_FILE_H;
extern const uint64_t BB_FILES[8];

extern const uint64_t BB_RANK_1;
extern const uint64_t BB_RANK_2;
extern const uint64_t BB_RANK_3;
extern const uint64_t BB_RANK_4;
extern const uint64_t BB_RANK_5;
extern const uint64_t BB_RANK_6;
extern const uint64_t BB_RANK_7;
extern const uint64_t BB_RANK_8;
extern const uint64_t BB_RANKS[8];

extern const uint64_t BB_DIAGONAL_1;
extern const uint64_t BB_DIAGONAL_2;
extern const uint64_t BB_DIAGONAL_3;
extern const uint64_t BB_DIAGONAL_4;
extern const uint64_t BB_DIAGONAL_5;
extern const uint64_t BB_DIAGONAL_6;
extern const uint64_t BB_DIAGONAL_7;
extern const uint64_t BB_DIAGONAL_8;
extern const uint64_t BB_DIAGONAL_9;
extern const uint64_t BB_DIAGONAL_10;
extern const uint64_t BB_DIAGONAL_11;
extern const uint64_t BB_DIAGONAL_12;
extern const uint64_t BB_DIAGONAL_13;
extern const uint64_t BB_DIAGONAL_14;
extern const uint64_t BB_DIAGONAL_15;
extern const uint64_t BB_DIAGONALS[15];

extern const uint64_t BB_ANTI_DIAGONAL_1;
extern const uint64_t BB_ANTI_DIAGONAL_2;
extern const uint64_t BB_ANTI_DIAGONAL_3;
extern const uint64_t BB_ANTI_DIAGONAL_4;
extern const uint64_t BB_ANTI_DIAGONAL_5;
extern const uint64_t BB_ANTI_DIAGONAL_6;
extern const uint64_t BB_ANTI_DIAGONAL_7;
extern const uint64_t BB_ANTI_DIAGONAL_8;
extern const uint64_t BB_ANTI_DIAGONAL_9;
extern const uint64_t BB_ANTI_DIAGONAL_10;
extern const uint64_t BB_ANTI_DIAGONAL_11;
extern const uint64_t BB_ANTI_DIAGONAL_12;
extern const uint64_t BB_ANTI_DIAGONAL_13;
extern const uint64_t BB_ANTI_DIAGONAL_14;
extern const uint64_t BB_ANTI_DIAGONAL_15;
extern const uint64_t BB_ANTI_DIAGONALS[15];

extern uint64_t BB_RAYS[64][64];
extern const Move NULL_MOVE;


void rays_init(void);

void* smalloc(size_t size);
void* scalloc(size_t n, size_t size);
void* srealloc(void* ptr, size_t size);

uint64_t get_ray_between(int square1, int square2);
uint64_t get_full_ray_on(int square1, int square2);

int parse_square(const char* square);
int parse_piece(char piece);
char* parse_move(Move move);

bool move_equals(Move move1, Move move2);

bool get_bit(uint64_t bb, int square);
void set_bit(uint64_t* bb, int square);
void clear_bit(uint64_t* bb, int square);
int pop_count(uint64_t bb);

void print_bb(uint64_t bb);
void print_move(Move move);

int rank_of(int square);
int file_of(int file);
int diagonal_of(int square);
int anti_diagonal_of(int square);

int get_lsb(uint64_t bb);
int pull_lsb(uint64_t* bb);

#undef max
int max(int x, int y);
#undef min
int min(int x, int y);


#endif
