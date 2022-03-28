#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <stdbool.h>


enum squares {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};


static const bool WHITE = true;
static const bool BLACK = false;


// Bitboards
static const uint64_t BB_SQUARES[64] = {
    1ULL << A1, 1ULL << B1, 1ULL << C1, 1ULL << D1, 1ULL << E1, 1ULL << F1, 1ULL << G1, 1ULL << H1,
    1ULL << A2, 1ULL << B2, 1ULL << C2, 1ULL << D2, 1ULL << E2, 1ULL << F2, 1ULL << G2, 1ULL << H2,
    1ULL << A3, 1ULL << B3, 1ULL << C3, 1ULL << D3, 1ULL << E3, 1ULL << F3, 1ULL << G3, 1ULL << H3,
    1ULL << A4, 1ULL << B4, 1ULL << C4, 1ULL << D4, 1ULL << E4, 1ULL << F4, 1ULL << G4, 1ULL << H4,
    1ULL << A5, 1ULL << B5, 1ULL << C5, 1ULL << D5, 1ULL << E5, 1ULL << F5, 1ULL << G5, 1ULL << H5,
    1ULL << A6, 1ULL << B6, 1ULL << C6, 1ULL << D6, 1ULL << E6, 1ULL << F6, 1ULL << G6, 1ULL << H6,
    1ULL << A7, 1ULL << B7, 1ULL << C7, 1ULL << D7, 1ULL << E7, 1ULL << F7, 1ULL << G7, 1ULL << H7,
    1ULL << A8, 1ULL << B8, 1ULL << C8, 1ULL << D8, 1ULL << E8, 1ULL << F8, 1ULL << G8, 1ULL << H8
};

static const uint64_t BB_ALL = 18446744073709551615ULL;

static const uint64_t BB_LIGHT_SQUARES = 6172840429334713770ULL;
static const uint64_t BB_DARK_SQUARES = 12273903644374837845ULL;

static const uint64_t BB_FILE_A = 72340172838076673ULL;
static const uint64_t BB_FILE_B = BB_FILE_A << 1;
static const uint64_t BB_FILE_C = BB_FILE_A << 2;
static const uint64_t BB_FILE_D = BB_FILE_A << 3;
static const uint64_t BB_FILE_E = BB_FILE_A << 4;
static const uint64_t BB_FILE_F = BB_FILE_A << 5;
static const uint64_t BB_FILE_G = BB_FILE_A << 6;
static const uint64_t BB_FILE_H = BB_FILE_A << 7;
static const uint64_t BB_FILES[8] = {BB_FILE_A, BB_FILE_B, BB_FILE_C, BB_FILE_D, BB_FILE_E, BB_FILE_F, BB_FILE_G, BB_FILE_H};

static const uint64_t BB_RANK_1 = 255ULL;
static const uint64_t BB_RANK_2 = BB_RANK_1 << 8;
static const uint64_t BB_RANK_3 = BB_RANK_1 << 16;
static const uint64_t BB_RANK_4 = BB_RANK_1 << 24;
static const uint64_t BB_RANK_5 = BB_RANK_1 << 32;
static const uint64_t BB_RANK_6 = BB_RANK_1 << 40;
static const uint64_t BB_RANK_7 = BB_RANK_1 << 48;
static const uint64_t BB_RANK_8 = BB_RANK_1 << 56;
static const uint64_t BB_RANKS[8] = {BB_RANK_1, BB_RANK_2, BB_RANK_3, BB_RANK_4, BB_RANK_5, BB_RANK_6, BB_RANK_7, BB_RANK_8};


#endif
