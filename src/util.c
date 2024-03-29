#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include "util.h"

extern _Thread_local Board board;
extern Info info;


const bool WHITE = true;
const bool BLACK = false;

const int MAX_PIECE_NUM = 32;

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
const uint64_t BB_FILE_B = 0x202020202020202;
const uint64_t BB_FILE_C = 0x404040404040404;
const uint64_t BB_FILE_D = 0x808080808080808;
const uint64_t BB_FILE_E = 0x1010101010101010;
const uint64_t BB_FILE_F = 0x2020202020202020;
const uint64_t BB_FILE_G = 0x4040404040404040;
const uint64_t BB_FILE_H = 0x8080808080808080;
const uint64_t BB_FILES[8] = {0x0101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808,
                              0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080};

const uint64_t BB_RANK_1 = 0xff;
const uint64_t BB_RANK_2 = 0xff00;
const uint64_t BB_RANK_3 = 0xff0000;
const uint64_t BB_RANK_4 = 0xff000000;
const uint64_t BB_RANK_5 = 0xff00000000;
const uint64_t BB_RANK_6 = 0xff0000000000;
const uint64_t BB_RANK_7 = 0xff000000000000;
const uint64_t BB_RANK_8 = 0xff00000000000000;
const uint64_t BB_RANKS[8] = {0xff, 0xff00, 0xff0000, 0xff000000,
                              0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000};

const uint64_t BB_DIAGONAL_1 = 0x80; // Numbered from lower right to upper left
const uint64_t BB_DIAGONAL_2 = 0x8040;
const uint64_t BB_DIAGONAL_3 = 0x804020;
const uint64_t BB_DIAGONAL_4 = 0x80402010;
const uint64_t BB_DIAGONAL_5 = 0x8040201008;
const uint64_t BB_DIAGONAL_6 = 0x804020100804;
const uint64_t BB_DIAGONAL_7 = 0x80402010080402;
const uint64_t BB_DIAGONAL_8 = 0x8040201008040201;
const uint64_t BB_DIAGONAL_9 = 0x4020100804020100;
const uint64_t BB_DIAGONAL_10 = 0x2010080402010000;
const uint64_t BB_DIAGONAL_11 = 0x1008040201000000;
const uint64_t BB_DIAGONAL_12 = 0x804020100000000;
const uint64_t BB_DIAGONAL_13 = 0x402010000000000;
const uint64_t BB_DIAGONAL_14 = 0x201000000000000;
const uint64_t BB_DIAGONAL_15 = 0x100000000000000;
const uint64_t BB_DIAGONALS[15] = {0x80, 0x8040, 0x804020, 0x80402010, 0x8040201008, 
                                   0x804020100804, 0x80402010080402, 0x8040201008040201, 0x4020100804020100,
                                   0x2010080402010000, 0x1008040201000000, 0x804020100000000,
                                   0x402010000000000, 0x201000000000000, 0x100000000000000};

const uint64_t BB_ANTI_DIAGONAL_1 = 0x1; // Numbered from lower left to upper right
const uint64_t BB_ANTI_DIAGONAL_2 = 0x102;
const uint64_t BB_ANTI_DIAGONAL_3 = 0x10204;
const uint64_t BB_ANTI_DIAGONAL_4 = 0x1020408;
const uint64_t BB_ANTI_DIAGONAL_5 = 0x102040810;
const uint64_t BB_ANTI_DIAGONAL_6 = 0x10204081020;
const uint64_t BB_ANTI_DIAGONAL_7 = 0x1020408102040;
const uint64_t BB_ANTI_DIAGONAL_8 = 0x102040810204080;
const uint64_t BB_ANTI_DIAGONAL_9 = 0x204081020408000;
const uint64_t BB_ANTI_DIAGONAL_10 = 0x408102040800000;
const uint64_t BB_ANTI_DIAGONAL_11 = 0x810204080000000;
const uint64_t BB_ANTI_DIAGONAL_12 = 0x1020408000000000;
const uint64_t BB_ANTI_DIAGONAL_13 = 0x2040800000000000;
const uint64_t BB_ANTI_DIAGONAL_14 = 0x4080000000000000;
const uint64_t BB_ANTI_DIAGONAL_15 = 0x8000000000000000;
const uint64_t BB_ANTI_DIAGONALS[15] = {0x1, 0x102, 0x10204, 0x1020408, 0x102040810, 0x10204081020, 
                                        0x1020408102040, 0x102040810204080, 0x204081020408000,
                                        0x408102040800000, 0x810204080000000, 0x1020408000000000,
                                        0x2040800000000000, 0x4080000000000000, 0x8000000000000000};

uint64_t BB_RAYS[64][64];
const Move NULL_MOVE = {A1, A1, PASS};


/**
 * Initalizes BB_RAYS[64][64] with all rays that connect from one square to another.
 * For example, there is a ray between a1 and c3, but not betweem a1 and b3.
 */
void rays_init(void) {
    for (int square1 = A1; square1 <= H8; square1++) {
        for (int square2 = A1; square2 <= H8; square2++) {
            if (square1 == square2) {
                BB_RAYS[square1][square2] = 0;
                continue;
            }

            uint64_t square2_bb = BB_SQUARES[square2];

            uint64_t rank = BB_RANKS[rank_of(square1)];
            if (rank & square2_bb) {
                BB_RAYS[square1][square2] = rank;
                continue;
            }

            uint64_t file = BB_FILES[file_of(square1)];
            if (file & square2_bb) {
                BB_RAYS[square1][square2] = file;
                continue;
            }

            uint64_t diagonal = BB_DIAGONALS[diagonal_of(square1)];
            if (diagonal & square2_bb) {
                BB_RAYS[square1][square2] = diagonal;
                continue;
            }

            uint64_t anti_diagonal = BB_ANTI_DIAGONALS[anti_diagonal_of(square1)];
            if (anti_diagonal & square2_bb) {
                BB_RAYS[square1][square2] = anti_diagonal;
                continue;
            }
            
            BB_RAYS[square1][square2] = 0;
        }
    }
}


/**
 * Malloc an amount of space equal to size safely,
 * exiting program on failure.
 * 
 * @param size size of the memory block to allocate.
 * @return void* to the beginning of the allocated memory.
 */
void* smalloc(size_t size) {
    void* ptr = malloc(size);
    assert(ptr);
    return ptr;
}


/**
 * Malloc an amount of space equal to size safely and set to 0,
 * exiting program on failure.
 * 
 * @param n how many spaces to initalize.
 * @param size the size of each space.
 * @return void* to the beginning of the allocated memory.
 */
void* scalloc(size_t n, size_t size) {
    void* ptr = calloc(n, size);
    assert(ptr);
    return ptr;
}


/**
 * Realloc an amount of space equal to size safely,
 * exiting program on failure.
 * 
 * @param ptr pointer to pointer to memory block previously malloced.
 * @param size new size of the memory block to allocate.
 * @return void* to the beginning of the allocated memory.
 */
void* srealloc(void* ptr, size_t size) {
    ptr = realloc(ptr, size);
    assert(ptr);
    return ptr;
}


/**
 * @param square1 
 * @param square2 
 * @return the bitboard of the ray between the two squares (inclusive), if any.
 *         returns empty bitboard if squares don't share a rank, file, diagonal, or anti-diagonal.
 */
uint64_t get_ray_between(int square1, int square2) {
    return (BB_RAYS[square1][square2] & ((BB_ALL << square1) ^ (BB_ALL << square2))) | BB_SQUARES[square2];
}


/**
 * @param square1 
 * @param square2 
 * @return the bitboard of the rank, file, diagonal, or anti-diagonal the two squares share.
 *         returns empty bitboard if squares don't share a rank, file, diagonal, or anti-diagonal.
 */
uint64_t get_full_ray_on(int square1, int square2) {
    return BB_RAYS[square1][square2];
}


/**
 * @param square the string of the square name, ie "a1".
 * @return the square's integer value.
 * https://www.chessprogramming.org/images/0/0d/BBUniverse.jpg
 */
int parse_square(const char* square) {
    int file = square[0] - 'a';
    int rank = square[1] - '0';
    return 8 * (rank - 1) + file;
}


/**
 * @param piece the char of the piece, ie 'K', 'B', 'q'.
 * @return [0-11] depending on the piece
 */
int parse_piece(char piece) {
    switch (piece) {
        case 'P':
            return 0;
        case 'N':
            return 1;
        case 'B':
            return 2;
        case 'R':
            return 3;
        case 'Q':
            return 4;
        case 'K':
            return 5;
        case 'p':
            return 6;
        case 'n':
            return 7;
        case 'b':
            return 8;
        case 'r':
            return 9;
        case 'q':
            return 10;
        case 'k':
            return 11;
        default:
            return 0; // en passant, index neutral
    }
}


/**
 * @param move 
 * @return the move in from square - to square notation.
 * Return needs to be freed.
 */
char* parse_move(Move move) {
    char* str = smalloc(5);
    str[0] = 'a' + file_of(move.from);
    str[1] = '1' + rank_of(move.from);
    str[2] = 'a' + file_of(move.to);
    str[3] = '1' + rank_of(move.to);

    switch (move.flag) {
        case PR_QUEEN:
            str[4] = 'q';
            str[5] = '\0';
            break;
        case PR_ROOK:
            str[4] = 'r';
            str[5] = '\0';
            break;
        case PR_KNIGHT:
            str[4] = 'n';
            str[5] = '\0';
            break;
        case PR_BISHOP:
            str[4] = 'b';
            str[5] = '\0';
            break;
        default:
            str[4] = '\0';
    }

    return str;
}


/**
 * @param move1 
 * @param move2
 * @return if move1 and move2 are the same move
 */
bool move_equals(Move move1, Move move2) {
    return (move1.from == move2.from && move1.to == move2.to && move1.flag == move1.flag);
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
 * Turn on the square on the bitboard.
 * @param bb the bitboard.
 * @param square 
 */
void set_bit(uint64_t* bb, int square) {
    *bb |= (1ULL << square);
}


/**
 * Clear the square on the bitboard.
 * @param bb the bitboard.
 * @param square 
 */
void clear_bit(uint64_t* bb, int square) {
    *bb &= ~(1ULL << square);
}


/**
 * @param bb
 * @return the number of set bits in the bitboard
 */
int pop_count(uint64_t bb) {
    return __builtin_popcountll(bb);
}


/**
 * Prints the binary representation of the bitboard.
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
 * Prints the move in from square - to square notation.
 * @param move 
 */
void print_move(Move move) {
    printf("%c", 'a' + file_of(move.from));
    printf("%d", rank_of(move.from) + 1);
    printf("%c", 'a' + file_of(move.to));
    printf("%d", rank_of(move.to) + 1);
    
    switch (move.flag) {
        case PR_QUEEN:
            printf("q");
            break;
        case PR_ROOK:
            printf("r");
            break;
        case PR_BISHOP:
            printf("b");
            break;
        case PR_KNIGHT:
            printf("n");
            break;
        case PC_QUEEN:
            printf("q");
            break;
        case PC_ROOK:
            printf("r");
            break;
        case PC_BISHOP:
            printf("b");
            break;
        case PC_KNIGHT:
            printf("n");
            break;
    }
}


/**
 * @param square
 * @return the rank of the square (0-7)
 */
int rank_of(int square) {
    return square / 8;
}


/**
 * @param square
 * @return the rank of the square (0-7)
 */
int file_of(int square) {
    return square % 8;
}


/**
 * @param square
 * @return the diagonal the square is on (0-7)
 */
int diagonal_of(int square) {
    return 7 + rank_of(square) - file_of(square);
}


/**
 * @param square
 * @return the anti-diagonal the square is on (0-7)
 */
int anti_diagonal_of(int square) {
    return rank_of(square) + file_of(square);
}


/**
 * @param bb 
 * @return the index of the least-significant bit from the bb, range [A1, H8].
 *         returns -1 if no bit is set
 */
int get_lsb(uint64_t bb) {
    return __builtin_ffsll(bb) - 1;
}


/**
 * removes the least-significant bit from the bitboard and returns it
 * @param bb 
 * @return the least-significant bit
 */
int pull_lsb(uint64_t* bb) {
    int square = get_lsb(*bb);
    *bb &= *bb - 1;
    return square;
}


/**
 * @param x 
 * @param y 
 * @return the larger of x, y 
 */
int max(int x, int y) {
    return (x >= y) ? x : y;
}


/**
 * @param x 
 * @param y 
 * @return the smaller of x, y 
 */
int min(int x, int y) {
    return (x <= y) ? x : y;
}
