#ifndef UTIL_H
#define UTIL_H

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


enum flags {
    INVALID, // Move is uninitialized
    NONE, // No special flag
    PASS, // Null move
    CAPTURE,
    EN_PASSANT,
    CASTLING,
    PROMOTION_KNIGHT,
    PROMOTION_BISHOP,
    PROMOTION_ROOK,
    PROMOTION_QUEEN
};


/**
 * Representation of a move using:
 * - square piece is moving from
 * - square piece is moving to
 * - any special characteristic of the move
 */
typedef struct Move {
    int from;
    int to;
    int flag;
} Move;


/**
 * Representation of the board using:
 * - mailbox representation for piece-at-square retrieval
 * - bitboards for every color and piece type
 * - bitboards of all occupied squares, and the occupied squares of just white/black
 * - array of all bitboards
 * - flag denoting whose turn it is
 * - flags for white/black castling rights kingside/queenside
 * - en passant target square, if any
 * - halfmove counter, denoting the number of halfmoves since the last capture or pawn advance
 * - fullmove counter, denoting the number of cycles of a white move and a black move
 * - a zobrist hash value for the current position
 */
typedef struct Board {
    char mailbox[64];

    uint64_t w_pawns;
    uint64_t w_knights;
    uint64_t w_bishops;
    uint64_t w_rooks;
    uint64_t w_queens;
    uint64_t w_king;
    uint64_t b_pawns;
    uint64_t b_knights;
    uint64_t b_bishops;
    uint64_t b_rooks;
    uint64_t b_queens;
    uint64_t b_king;

    uint64_t occupied;
    uint64_t w_occupied;
    uint64_t b_occupied;

    uint64_t* bitboards[12];

    bool turn;

    bool w_kingside_castling_rights;
    bool w_queenside_castling_rights;
    bool b_kingside_castling_rights;
    bool b_queenside_castling_rights;

    int en_passant_square;

    int halfmove_clock;
    int fullmove_number;

    uint64_t zobrist;
} Board;


/**
 * A stack of all the board positions that's been reached and
 * the moves that got to them.
 */
typedef struct Stack {
    Board board;
    Move move;
    struct Stack *next;
} Stack;


extern const bool WHITE;
extern const bool BLACK;

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

extern uint64_t ZOBRIST_VALUES[781];

extern const Move NULL_MOVE;
extern const int NULL_SQUARE;


int parse_square(char* square);
int parse_piece(char piece);

bool get_bit(uint64_t bb, int square);
void set_bit(uint64_t* bb, int square);
void clear_bit(uint64_t* bb, int square);
int pop_count(uint64_t bb);

void print_bb(uint64_t bb);
void print_mailbox(char* mailbox);
void print_move_pre(Board* board, Move move);
void print_move_post(Board* board, Move move);

int rank_of(int square);
int file_of(int file);
int diagonal_of(int square);
int anti_diagonal_of(int square);

uint64_t get_reverse_bb(uint64_t bb);

int get_lsb(uint64_t bb);
int pull_lsb(uint64_t* bb);

void init_rays(void);
uint64_t get_ray_between(int square1, int square2);
static uint64_t _get_ray(int square1, int square2);

void init_zobrist_table(void);

uint64_t rand_ull(void);


#endif
