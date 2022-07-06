#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "util.h"


extern Board board;
extern Stack* stack;
extern RTable rtable;


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
const uint64_t BB_DIAGONALS[15] = {BB_DIAGONAL_1, BB_DIAGONAL_2, BB_DIAGONAL_3, BB_DIAGONAL_4, BB_DIAGONAL_5, BB_DIAGONAL_6, 
                                   BB_DIAGONAL_7, BB_DIAGONAL_8, BB_DIAGONAL_9, BB_DIAGONAL_10, BB_DIAGONAL_11, BB_DIAGONAL_12,
                                   BB_DIAGONAL_13, BB_DIAGONAL_14, BB_DIAGONAL_15};

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
const uint64_t BB_ANTI_DIAGONALS[15] = {BB_ANTI_DIAGONAL_1, BB_ANTI_DIAGONAL_2, BB_ANTI_DIAGONAL_3, BB_ANTI_DIAGONAL_4, BB_ANTI_DIAGONAL_5, BB_ANTI_DIAGONAL_6, 
                                        BB_ANTI_DIAGONAL_7, BB_ANTI_DIAGONAL_8, BB_ANTI_DIAGONAL_9, BB_ANTI_DIAGONAL_10, BB_ANTI_DIAGONAL_11, BB_ANTI_DIAGONAL_12,
                                        BB_ANTI_DIAGONAL_13, BB_ANTI_DIAGONAL_14, BB_ANTI_DIAGONAL_15};

uint64_t BB_RAYS[64][64];

/**
 * - 000-767: numbers for each piece on each square
 * -     768: number to indicate side to move is black
 * - 769-772: numbers for castling rights
 * - 773-780: numbers to indicate en passant file
 */
uint64_t ZOBRIST_VALUES[781];

// Misc
const Move NULL_MOVE = {A1, A1, PASS};
const int INVALID = -1;
const int MAX_MOVE_NUM = 219; // 218 + invalid move to signal end of list


/**
 * Initalizes BB_RAYS[64][64] with all rays that connect from one square to another
 * (see _get_ray())
 */
void init_rays(void) {
    for (int square1 = A1; square1 <= H8; square1++) {
        for (int square2 = A1; square2 <= H8; square2++) {
            BB_RAYS[square1][square2] = _get_ray(square1, square2);
        }
    }
}


/**
 * @param square1 
 * @param square2 
 * @return the bitboard of the ray between the two squares (including the squares), if any.
 */
uint64_t get_ray_between(int square1, int square2) {
    return (BB_RAYS[square1][square2] & ((BB_ALL << square1) ^ (BB_ALL << square2))) | BB_SQUARES[square2];
}


/**
 * @param square1 
 * @param square2 
 * @return the bitboard of the rank, file, diagonal, or anti-diagonal between the two squares, if any.
 */
uint64_t get_full_ray_between(int square1, int square2) {
    return BB_RAYS[square1][square2];
}


/**
 * @param square1 
 * @param square2 
 * @return the ray (rank, file, or diagonal) that connects the two squares, if any.
 *         for example, there is a ray between a1 and c3, but not betweem a1 and b3.
 *         returns empty bitboard if the two squares are equal
 */
static uint64_t _get_ray(int square1, int square2) {
    if (square1 == square2) return 0;

    uint64_t square2_bb = BB_SQUARES[square2];

    uint64_t rank = BB_RANKS[rank_of(square1)];
    if (rank & square2_bb) return rank;

    uint64_t file = BB_FILES[file_of(square1)];
    if (file & square2_bb) return file;

    uint64_t diagonal = BB_DIAGONALS[diagonal_of(square1)];
    if (diagonal & square2_bb) return diagonal;

    uint64_t anti_diagonal = BB_ANTI_DIAGONALS[anti_diagonal_of(square1)];
    if (anti_diagonal & square2_bb) return anti_diagonal;
    
    return 0;
}


/**
 * Initalizes ZOBRIST_VALUES[781] with random unsigned 64-bit integers.
 * - 768 numbers for each piece on each square
 * - 1 number to indicate side to move is black
 * - 4 numbers for castling rights
 * - 8 numbers to indicate en passant file
 */
void init_zobrist_table(void) {
    for (int i = 0; i < 781; i++) {
        ZOBRIST_VALUES[i] = _rand_ull();
    }
}


/**
 * @return a random unsigned 64-bit integer.
 * @author https://stackoverflow.com/a/28116032.
 */
static uint64_t _rand_ull(void) {
    uint64_t n = 0;
    for (int i = 0; i < 5; i++) {
        n = (n << 15) | (rand() & 0x7FFF);
    }
    return n & 0xFFFFFFFFFFFFFFFF;
}


/**
 * @param square the string of the square name, ie "a1".
 * @return the square's integer value.
 * https://www.chessprogramming.org/images/0/0d/BBUniverse.jpg
 */
int parse_square_char(char* square) {
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
            return INVALID;
    }
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
        case PROMOTION_QUEEN:
            printf("Q");
            break;
        case PROMOTION_ROOK:
            printf("R");
            break;
        case PROMOTION_KNIGHT:
            printf("N");
            break;
        case PROMOTION_BISHOP:
            printf("B");
            break;
    }
}


/**
 * Prints the move (before its been made) in algebraic notation.
 * Does not support disambiguating moves.
 * @param move the move to be made.
 */
void printf_move_pre(Move move) {
    char piece = toupper(board.mailbox[move.from]);

    if (piece == 'P') {
        printf("%c", 104 - (7 - file_of(move.to)));
        printf("%d", rank_of(move.to) + 1);

        switch (move.flag) {
            case PROMOTION_QUEEN:
                printf("Q");
                break;
            case PROMOTION_ROOK:
                printf("R");
                break;
            case PROMOTION_KNIGHT:
                printf("N");
                break;
            case PROMOTION_BISHOP:
                printf("B");
                break;
        }
    } else {
        printf("%c", piece);
        printf("%c", 104 - (7 - file_of(move.to)));
        printf("%d", rank_of(move.to) + 1);
    }
}


/**
 * Prints the move (after its been made) in algebraic notation.
 * Does not support disambiguating moves.
 * @param move the move that has just been made.
 */
void printf_move_post(Move move) {
    switch (move.flag) {
        case PROMOTION_QUEEN:
            printf("%c", 104 - (7 - file_of(move.to)));
            printf("%d", rank_of(move.to) + 1);
            printf("Q");
            break;
        case PROMOTION_ROOK:
            printf("%c", 104 - (7 - file_of(move.to)));
            printf("%d", rank_of(move.to) + 1);
            printf("R");
            break;
        case PROMOTION_KNIGHT:
            printf("%c", 104 - (7 - file_of(move.to)));
            printf("%d", rank_of(move.to) + 1);
            printf("N");
            break;
        case PROMOTION_BISHOP:
            printf("%c", 104 - (7 - file_of(move.to)));
            printf("%d", rank_of(move.to) + 1);
            printf("B");
            break;
        default:
            char piece = toupper(board.mailbox[move.to]);
            if (piece != 'P') printf("%c", piece);
            printf("%c", 104 - (7 - file_of(move.to)));
            printf("%d", rank_of(move.to) + 1);
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
