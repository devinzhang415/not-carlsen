#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdint.h>
#include "board.h"


/**
 * @brief Representation of a move using:
 * - square piece is moving from
 * - square piece is moving to
 * - any special characteristic of the move, such as the promotion piece / castling / en passant
 */
typedef struct Move {
    int from;
    int to;
    int flag;
} Move;


extern const uint64_t BB_KNIGHT_ATTACKS[64];
extern uint64_t BB_BISHOP_ATTACKS[64][512];
extern uint64_t BB_ROOK_ATTACKS[64][4096];
extern const uint64_t BB_KING_ATTACKS[64];


extern const uint64_t BISHOP_MAGICS[64];
extern const uint64_t ROOK_MAGICS[64];
extern uint64_t BB_BISHOP_ATTACK_MASKS[64];
extern uint64_t BB_ROOK_ATTACK_MASKS[64];
extern uint64_t ROOK_ATTACK_SHIFTS[64];
extern uint64_t BISHOP_ATTACK_SHIFTS[64];


void init_bishop_attacks(void);
void init_rook_attacks(void);


uint64_t _get_pawn_moves_all(Board *board, bool color);
uint64_t _get_knight_moves(Board *board, bool color, int square);
uint64_t _get_bishop_moves(Board *board, bool color, int square);
uint64_t _get_rook_moves(Board *board, bool color, int square);
uint64_t _get_queen_moves(Board *board, bool color, int square);
uint64_t _get_king_moves(Board *board, bool color, int square);

uint64_t _get_pawn_captures_all(Board *board, bool color);


uint64_t _get_checkmask(Board *board, bool color);
uint64_t _get_pinmask(Board *board, bool color);


uint64_t _init_bishop_attacks_helper(int square, uint64_t subset);
uint64_t _init_rook_attacks_helper(int square, uint64_t subset);


#endif