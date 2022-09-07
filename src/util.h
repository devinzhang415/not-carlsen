#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>


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


// Special characteristic of a move
enum move_flags {
    NONE, // No special flag
    PASS, // Null move
    CAPTURE,
    EN_PASSANT,
    CASTLING,
    PR_KNIGHT,
    PR_BISHOP,
    PR_ROOK,
    PR_QUEEN,
    PC_KNIGHT, // Promotion that is also a capture
    PC_BISHOP,
    PC_ROOK,
    PC_QUEEN
};


// Transposition score types
enum tt_flags {
    EXACT,
    LOWERBOUND,
    UPPERBOUND
};


/**
 * Representation of a move.
 * 
 * TODO
 * Bitfields dont seem to be faster?
 */
typedef struct Move {
    unsigned int from : 6; // square piece is moving from
    unsigned int to : 6; // square piece is moving to
    unsigned int flag : 4; // any special characteristic of the move
} Move;


/**
 * Representation of the board
 */
typedef struct Board {
    char mailbox[64]; // piece-centric board representation

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

    int w_king_square;
    int b_king_square;

    bool turn;

    bool w_kingside_castling_rights;
    bool w_queenside_castling_rights;
    bool b_kingside_castling_rights;
    bool b_queenside_castling_rights;

    int en_passant_square; // en passant target square, if any

    int halfmove_clock; // number of halfmoves since the last capture or pawn advance
    int fullmove_number; // number of cycles of a white move and a black move

    uint64_t zobrist; // zobrist hash value for the current position
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


/**
 * Hashtable entry to store transposition of a position
 * (a previous stored result).
 */
typedef struct TTable_Entry {
    uint64_t key; // key = zobrist ^ score for lockless access
    int depth;
    Move move;
    int score;
    int flag;
    bool initialized; // deleted or not
} TTable_Entry;


/**
 * Lockless transposition hashtable structure.
 */
typedef struct TTable {
    uint64_t size;
    uint64_t capacity;
    TTable_Entry* entries;
} TTable;


/**
 * Hashtable entry to store number of times position has occured for
 * threefold repetition purposes.
 */
typedef struct RTable_Entry {
    uint64_t key;
    int num;
    bool initialized; // deleted or not
} RTable_Entry;


/**
 * Threefold repetition hashtable structure.
 */
typedef struct RTable {
    uint64_t size;
    uint64_t capacity;
    RTable_Entry* entries;
} RTable;


/**
 * Parameters to search with for UCI.
 * Descriptions from http://wbec-ridderkerk.nl/html/UCIProtocol.html
 * 
 * TODO missing commands:
 * - searchmoves
 * - ponder
 * - mate
 * - infinite
 */
typedef struct Info {
    clock_t wtime; // white has x msec left on the clock
    clock_t btime; // black has x msec left on the clock
    clock_t winc; // white increment per move in mseconds if x > 0
    clock_t binc; // black increment per move in mseconds if x > 0
    int movestogo; // there are x moves to the next time control
    int depth; // search x plies only
    int nodes; // search x nodes only 
    clock_t movetime; // search exactly x mseconds
} Info;


/**
 * 
 */
typedef struct Param {
    Board* board;
    Stack** stack;
    RTable* rtable;

    int depth;
    int alpha;
    int beta;
    bool pv_node;
    bool color;
    clock_t start;
    uint64_t* nodes;
    Move* pv;
} Param;


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
extern const int INVALID;
extern const double MAX_LOAD_FACTOR;
extern const int MATE_SCORE;
extern const int MAX_DEPTH;
extern const int MAX_MOVE_NUM;
extern const int MAX_CAPTURE_NUM;


uint64_t get_ray_between(int square1, int square2);
uint64_t get_ray_between_inclusive(int square1, int square2);

int parse_square(char* square);
int parse_piece(char piece);
char* parse_move(Move move);

bool get_bit(uint64_t bb, int square);
void set_bit(uint64_t* bb, int square);
void clear_bit(uint64_t* bb, int square);
int pop_count(uint64_t bb);

void print_bb(uint64_t bb);
void print_move(Move move);
void print_info(int depth, int score, uint64_t nodes, double time, Move* pv);

static int _save_move_str(char* str, int i, Move move);

int rank_of(int square);
int file_of(int file);
int diagonal_of(int square);
int anti_diagonal_of(int square);

int get_lsb(uint64_t bb);
int pull_lsb(uint64_t* bb);

int max(int x, int y);
int min(int x, int y);


#endif
