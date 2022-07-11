#ifndef EVALUATE_H
#define EVALUATE_H

#include <stdbool.h>
#include "util.h"


extern const double MATERIAL_WEIGHT;
extern const double PSQT_WEIGHT;

extern const int PHASE_VALUES[6];

extern const int MATERIAL_VALUES[6];

extern const int W_MG_PAWN_TABLE[64];
extern const int W_MG_KNIGHT_TABLE[64];
extern const int W_MG_BISHOP_TABLE[64];
extern const int W_MG_ROOK_TABLE[64];
extern const int W_MG_QUEEN_TABLE[64];
extern const int W_MG_KING_TABLE[64];
extern const int B_MG_PAWN_TABLE[64];
extern const int B_MG_KNIGHT_TABLE[64];
extern const int B_MG_BISHOP_TABLE[64];
extern const int B_MG_ROOK_TABLE[64];
extern const int B_MG_QUEEN_TABLE[64];
extern const int B_MG_KING_TABLE[64];

extern const int W_EG_PAWN_TABLE[64];
extern const int W_EG_KNIGHT_TABLE[64];
extern const int W_EG_BISHOP_TABLE[64];
extern const int W_EG_ROOK_TABLE[64];
extern const int W_EG_QUEEN_TABLE[64];
extern const int W_EG_KING_TABLE[64];
extern const int B_EG_PAWN_TABLE[64];
extern const int B_EG_KNIGHT_TABLE[64];
extern const int B_EG_BISHOP_TABLE[64];
extern const int B_EG_ROOK_TABLE[64];
extern const int B_EG_QUEEN_TABLE[64];
extern const int B_EG_KING_TABLE[64];

extern const int* MG_PSQTS[12];
extern const int* EG_PSQTS[12];


int eval(bool color);


#endif
