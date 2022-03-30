#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "util.h"


const bool WHITE = true;
const bool BLACK = false;


// Bitboards
const uint64_t BB_SQUARES[64] = {
    1ULL << A1, 1ULL << B1, 1ULL << C1, 1ULL << D1, 1ULL << E1, 1ULL << F1, 1ULL << G1, 1ULL << H1,
    1ULL << A2, 1ULL << B2, 1ULL << C2, 1ULL << D2, 1ULL << E2, 1ULL << F2, 1ULL << G2, 1ULL << H2,
    1ULL << A3, 1ULL << B3, 1ULL << C3, 1ULL << D3, 1ULL << E3, 1ULL << F3, 1ULL << G3, 1ULL << H3,
    1ULL << A4, 1ULL << B4, 1ULL << C4, 1ULL << D4, 1ULL << E4, 1ULL << F4, 1ULL << G4, 1ULL << H4,
    1ULL << A5, 1ULL << B5, 1ULL << C5, 1ULL << D5, 1ULL << E5, 1ULL << F5, 1ULL << G5, 1ULL << H5,
    1ULL << A6, 1ULL << B6, 1ULL << C6, 1ULL << D6, 1ULL << E6, 1ULL << F6, 1ULL << G6, 1ULL << H6,
    1ULL << A7, 1ULL << B7, 1ULL << C7, 1ULL << D7, 1ULL << E7, 1ULL << F7, 1ULL << G7, 1ULL << H7,
    1ULL << A8, 1ULL << B8, 1ULL << C8, 1ULL << D8, 1ULL << E8, 1ULL << F8, 1ULL << G8, 1ULL << H8
};

const uint64_t BB_ALL = 0xffffffffffffffff;

const uint64_t BB_LIGHT_SQUARES = 0x55aa55aa55aa55aa;
const uint64_t BB_DARK_SQUARES = 0xaa55aa55aa55aa55;

const uint64_t BB_FILE_A = 0x0101010101010101;
const uint64_t BB_FILE_B = BB_FILE_A << 1;
const uint64_t BB_FILE_C = BB_FILE_A << 2;
const uint64_t BB_FILE_D = BB_FILE_A << 3;
const uint64_t BB_FILE_E = BB_FILE_A << 4;
const uint64_t BB_FILE_F = BB_FILE_A << 5;
const uint64_t BB_FILE_G = BB_FILE_A << 6;
const uint64_t BB_FILE_H = BB_FILE_A << 7;
const uint64_t BB_FILES[8] = {BB_FILE_A, BB_FILE_B, BB_FILE_C, BB_FILE_D, BB_FILE_E, BB_FILE_F, BB_FILE_G, BB_FILE_H};

const uint64_t BB_RANK_1 = 0xff;
const uint64_t BB_RANK_2 = BB_RANK_1 << 8;
const uint64_t BB_RANK_3 = BB_RANK_1 << 16;
const uint64_t BB_RANK_4 = BB_RANK_1 << 24;
const uint64_t BB_RANK_5 = BB_RANK_1 << 32;
const uint64_t BB_RANK_6 = BB_RANK_1 << 40;
const uint64_t BB_RANK_7 = BB_RANK_1 << 48;
const uint64_t BB_RANK_8 = BB_RANK_1 << 56;
const uint64_t BB_RANKS[8] = {BB_RANK_1, BB_RANK_2, BB_RANK_3, BB_RANK_4, BB_RANK_5, BB_RANK_6, BB_RANK_7, BB_RANK_8};


/**
 * @param square the string of the square name, ie "A1."
 * @return the square's integer value.
 */
int parse_square(char *square) {
    int file = square[0] - 'a';
    int rank = square[1] - '0';
    return 8 * (rank - 1) + file;
}


/**
 * @param bb the bitboard.
 * @param square the square or indice of the bit.
 * @return the bit from the bitboard at the given square.
 */
bool get_bit(uint64_t bb, int square) {
    return bb & (1ULL << square);
}


/**
 * @brief Prints the binary representation of the bitboard.
 * 
 * @param bb the bitboard.
 */
void print_bb(uint64_t bb) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file <= 7; file++) {
            printf("%d ", get_bit(bb, 8*rank + file));
        }
        printf("\n");
    }
    printf("\n");
}


/**
 * @param square the square.
 * @return the rank of the square (0-7).
 */
int get_rank(int square) {
    return square / 8;
}


/**
 * @param square the square.
 * @return the rank of the square (0-7).
 */
int get_file(int square) {
    return square % 8;
}


/**
 * @param bb the bitboard.
 * @return the number of set bits in the bitboard.
 */
int pop_count(uint64_t bb) {
    return __builtin_popcountll(bb);
}
