#include <stdint.h>
#include "movegen.h"
#include "util.h"
#include "board.h"


// Pseudo-legal bitboards indexed by square to determine where that piece can attack
const uint64_t BB_KNIGHT_ATTACKS[64] = {
    0x20400, 0x50800, 0xa1100, 0x142200, 0x284400,
    0x508800, 0xa01000, 0x402000, 0x2040004, 0x5080008,
    0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010,
    0x40200020, 0x204000402, 0x508000805, 0xa1100110a, 0x1422002214,
    0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040, 0x20400040200,
    0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000,
    0xa0100010a000, 0x402000204000, 0x2040004020000, 0x5080008050000, 0xa1100110a0000,
    0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000,
    0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000,
    0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000, 0x400040200000000, 0x800080500000000,
    0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000,
    0x2000204000000000, 0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000,
    0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000
};

uint64_t BB_BISHOP_ATTACKS[64][512];

uint64_t BB_ROOK_ATTACKS[64][4096];

const uint64_t BB_KING_ATTACKS[64] = {
	0x302, 0x705, 0xe0a, 0x1c14, 0x3828,
    0x7050, 0xe0a0, 0xc040, 0x30203, 0x70507,
    0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0,
    0xc040c0, 0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00,
	0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000, 0x302030000,
    0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000,
    0xe0a0e00000, 0xc040c00000, 0x30203000000, 0x70507000000, 0xe0a0e000000,
    0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000,
	0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000,
    0x70507000000000, 0xe0a0e000000000, 0xc040c000000000, 0x302030000000000, 0x705070000000000,
    0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000,
    0xc040c00000000000, 0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000,
	0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000,
};


// Rook and bishop magic numbers to generate their magic bitboards
const uint64_t BISHOP_MAGICS[64] = {
	0x2020202020200, 0x2020202020000, 0x4010202000000, 0x4040080000000, 0x1104000000000,
    0x821040000000, 0x410410400000, 0x104104104000, 0x40404040400, 0x20202020200,
    0x40102020000, 0x40400800000, 0x11040000000, 0x8210400000, 0x4104104000,
    0x2082082000, 0x4000808080800, 0x2000404040400, 0x1000202020200, 0x800802004000,
	0x800400a00000, 0x200100884000, 0x400082082000, 0x200041041000, 0x2080010101000,
    0x1040008080800, 0x208004010400, 0x404004010200, 0x840000802000, 0x404002011000,
    0x808001041000, 0x404000820800, 0x1041000202000, 0x820800101000, 0x104400080800,
    0x20080080080, 0x404040040100, 0x808100020100, 0x1010100020800, 0x808080010400,
	0x820820004000, 0x410410002000, 0x82088001000, 0x2011000800, 0x80100400400,
    0x1010101000200, 0x2020202000400, 0x1010101000200, 0x410410400000, 0x208208200000,
    0x2084100000, 0x20880000, 0x1002020000, 0x40408020000, 0x4040404040000,
    0x2020202020000, 0x104104104000, 0x2082082000, 0x20841000, 0x208800,
	0x10020200, 0x404080200, 0x40404040400, 0x2020202020200
};

const uint64_t ROOK_MAGICS[64] = {
	0x80001020400080, 0x40001000200040, 0x80081000200080, 0x80040800100080, 0x80020400080080,
    0x80010200040080, 0x80008001000200, 0x80002040800100, 0x800020400080, 0x400020005000,
    0x801000200080, 0x800800100080, 0x800400080080, 0x800200040080, 0x800100020080,
    0x800040800100, 0x208000400080, 0x404000201000, 0x808010002000, 0x808008001000,
	0x808004000800, 0x808002000400, 0x10100020004, 0x20000408104, 0x208080004000,
    0x200040005000, 0x100080200080, 0x80080100080, 0x40080080080, 0x20080040080,
    0x10080800200, 0x800080004100, 0x204000800080, 0x200040401000, 0x100080802000,
    0x80080801000, 0x40080800800, 0x20080800400, 0x20001010004, 0x800040800100,
	0x204000808000, 0x200040008080, 0x100020008080, 0x80010008080, 0x40008008080,
    0x20004008080, 0x10002008080, 0x4081020004, 0x204000800080, 0x200040008080,
    0x100020008080, 0x80010008080, 0x40008008080, 0x20004008080, 0x800100020080,
    0x800041000080, 0xfffcddfced714a, 0x7ffcddfced714a, 0x3fffcdffd88096, 0x40810002101,
	0x1000204080011, 0x1000204000801, 0x1000082000401, 0x1fffaabfad1a2
};

// Attack masks and shifts for magic bitboard move generation
uint64_t BB_BISHOP_ATTACK_MASKS[64];
uint64_t BB_ROOK_ATTACK_MASKS[64];
uint64_t ROOK_ATTACK_SHIFTS[64];
uint64_t BISHOP_ATTACK_SHIFTS[64];


/**
 * @param board
 * @param piece_color the color of the pawns
 * @return where all the pawns can move
 */
uint64_t get_pawn_moves_all(Board *board, bool piece_color) {
    if (piece_color == WHITE) {
        uint64_t pawns = board->w_pawns;

        uint64_t single_push = pawns << 8;
        uint64_t double_push = (pawns & BB_RANK_2) << 16;
        uint64_t pushes = (single_push | double_push) & ~board->occupied;

        uint64_t captures = (((pawns << 9) & ~BB_FILE_A) | ((pawns << 7) & ~BB_FILE_H)) & board->b_occupied;
        if (board->en_passant_square != -1) {
            uint64_t ep_pawns = pawns & BB_RANK_5;
            uint64_t ep_captures = (((ep_pawns << 9) & ~BB_FILE_A) | ((ep_pawns << 7) & ~BB_FILE_H)) & BB_SQUARES[board->en_passant_square];
            captures |= ep_captures;
        }

        return pushes | captures;
    } else {
        uint64_t pawns = board->b_pawns;

        uint64_t single_push = pawns >> 8;
        uint64_t double_push = (pawns & BB_RANK_7) >> 16;
        uint64_t pushes = (single_push | double_push) & ~board->occupied;

        uint64_t captures = (((pawns >> 9) & ~BB_FILE_H) | ((pawns >> 7) & ~BB_FILE_A)) & board->w_occupied;
        if (board->en_passant_square != -1) {
            uint64_t ep_pawns = pawns & BB_RANK_4;
            uint64_t ep_captures = (((ep_pawns >> 9) & ~BB_FILE_H) | ((ep_pawns >> 7) & ~BB_FILE_A)) & BB_SQUARES[board->en_passant_square];
            captures |= ep_captures;
        }

        return pushes | captures;
    }
}


/**
 * @param board
 * @param square the square the knight is on
 * @param piece_color the color of the knight
 * @return where the knight can move from the given square
 */
uint64_t get_knight_moves(Board *board, int square, bool piece_color) {
    uint64_t moves = BB_KNIGHT_ATTACKS[square];

    return (piece_color == WHITE) ? moves & ~board->w_occupied : moves & ~board->b_occupied;
}


/**
 * @param board
 * @param square the square the bishop is on
 * @param piece_color the color of the bishop
 * @return where the bishop can move from the given square
 */
uint64_t get_bishop_moves(Board *board, int square, bool piece_color) {
    uint64_t occupied = board->occupied & BB_BISHOP_ATTACK_MASKS[square];
    uint64_t key = (occupied * BISHOP_MAGICS[square]) >> BISHOP_ATTACK_SHIFTS[square];
    uint64_t moves = BB_BISHOP_ATTACKS[square][key];

    return (piece_color == WHITE) ? moves & ~board->w_occupied : moves & ~board->b_occupied;
}


/**
 * @param board
 * @param square the square the bishop is on
 * @param piece_color the color of the bishop
 * @return where the bishop can move from the given square
 */
uint64_t get_rook_moves(Board *board, int square, bool piece_color) {
    uint64_t occupied = board->occupied & BB_ROOK_ATTACK_MASKS[square];
    uint64_t key = (occupied * ROOK_MAGICS[square]) >> ROOK_ATTACK_SHIFTS[square];
    uint64_t moves = BB_ROOK_ATTACKS[square][key];

    return (piece_color == WHITE) ? moves & ~board->w_occupied : moves & ~board->b_occupied;
}


/**
 * @param board
 * @param square the square the queen is on
 * @param piece_color the color of the queen
 * @return where the queen can move from the given square
 */
uint64_t get_queen_moves(Board *board, int square, bool piece_color) {
    uint64_t bishop_occupied = board->occupied & BB_BISHOP_ATTACK_MASKS[square];
    uint64_t bishop_key = (bishop_occupied * BISHOP_MAGICS[square]) >> BISHOP_ATTACK_SHIFTS[square];
    uint64_t bishop_moves = BB_BISHOP_ATTACKS[square][bishop_key];

    uint64_t rook_occupied = board->occupied & BB_ROOK_ATTACK_MASKS[square];
    uint64_t rook_key = (rook_occupied * ROOK_MAGICS[square]) >> ROOK_ATTACK_SHIFTS[square];
    uint64_t rook_moves = BB_ROOK_ATTACKS[square][rook_key];

    uint64_t moves = bishop_moves | rook_moves;

    return (piece_color == WHITE) ? moves & ~board->w_occupied : moves & ~board->b_occupied;
}


/**
 * @param board
 * @param square the square the king is on
 * @param piece_color the color of the king
 * @return where the king can move from the given square, including castling squares
 */
uint64_t get_king_moves(Board *board, int square, bool piece_color) {
    uint64_t moves = BB_KING_ATTACKS[square];
    if (piece_color == WHITE) {
        if (board->w_kingside_castling_rights) moves |= 1ULL << G1;
        if (board->w_queenside_castling_rights) moves |= 1ULL << C1;
        return moves & ~board->w_occupied;
    } else {
        if (board->b_kingside_castling_rights) moves |= 1ULL << G8;
        if (board->b_queenside_castling_rights) moves |= 1ULL << C8;
        return moves & ~board->b_occupied;
    }
}


/**
 * @brief Initalizes the bishop attack magic bitboard
 * @author github.com/nkarve
 */
void init_bishop_attacks(void) {
    for (int square = A1; square <= H8; square++) {
        uint64_t edges = ((BB_RANK_1 | BB_RANK_8) & ~BB_RANKS[rank_of(square)]) |
                         ((BB_FILE_A | BB_FILE_H) & ~BB_FILES[file_of(square)]);
        BB_BISHOP_ATTACK_MASKS[square] = (BB_DIAGONALS[diagonal_of(square)] ^ BB_ANTI_DIAGONALS[anti_diagonal_of(square)]) & ~edges;
        uint64_t attack_mask = BB_BISHOP_ATTACK_MASKS[square];

        int shift = 64 - pop_count(attack_mask);
        BISHOP_ATTACK_SHIFTS[square] = shift;

        uint64_t subset = 0;
        do {
            uint64_t index = subset;
            index *= BISHOP_MAGICS[square];
            index >>= shift;
            BB_BISHOP_ATTACKS[square][index] = _init_bishop_attacks_helper(square, subset);
            subset = (subset - attack_mask) & attack_mask;
        } while (subset);
    }
}


/**
 * @brief Initalizes the rook attack magic bitboard
 * @author github.com/nkarve
 */
void init_rook_attacks(void) {
    for (int square = A1; square <= H8; square++) {
        uint64_t edges = ((BB_RANK_1 | BB_RANK_8) & ~BB_RANKS[rank_of(square)]) |
                         ((BB_FILE_A | BB_FILE_H) & ~BB_FILES[file_of(square)]);
        BB_ROOK_ATTACK_MASKS[square] = (BB_RANKS[rank_of(square)] ^ BB_FILES[file_of(square)]) & ~edges;
        uint64_t attack_mask = BB_ROOK_ATTACK_MASKS[square];

        int shift = 64 - pop_count(attack_mask);
        ROOK_ATTACK_SHIFTS[square] = shift;

        uint64_t subset = 0;
        do {
            uint64_t index = subset;
            index *= ROOK_MAGICS[square];
            index >>= shift;
            BB_ROOK_ATTACKS[square][index] = _init_rook_attacks_helper(square, subset);
            subset = (subset - attack_mask) & attack_mask;
        } while (subset);
    }
}


/**
 * @brief Helper method to initalizes the bishop attack magic bitboard
 * @param square the current square
 * @param subset the current occupancy
 * @param attack_mask the bishop's attack mask without edges
 * @return the bishop attack bitboard
 * @author github.com/nkarve
 */
uint64_t _init_bishop_attacks_helper(int square, uint64_t subset) {
    uint64_t square_mask = BB_SQUARES[square];
    uint64_t diagonal_mask = BB_DIAGONALS[diagonal_of(square)];
    uint64_t anti_diagonal_mask = BB_ANTI_DIAGONALS[anti_diagonal_of(square)];

    uint64_t diagonal_attacks = (((diagonal_mask & subset) - square_mask * 2) ^
                            get_reverse_bb(get_reverse_bb(diagonal_mask & subset) - get_reverse_bb(square_mask) * 2)) &
                            diagonal_mask;

    uint64_t anti_diagonal_attacks = (((anti_diagonal_mask & subset) - square_mask * 2) ^
                            get_reverse_bb(get_reverse_bb(anti_diagonal_mask & subset) - get_reverse_bb(square_mask) * 2)) &
                            anti_diagonal_mask;

    return diagonal_attacks | anti_diagonal_attacks;
}


/**
 * @brief Helper method to initalizes the rook attack magic bitboard
 * @param square the current square
 * @param subset the current occupancy
 * @param attack_mask the rook's attack mask without edges
 * @return the rook attack bitboard
 * @author github.com/nkarve
 */
uint64_t _init_rook_attacks_helper(int square, uint64_t subset) {
    uint64_t square_mask = BB_SQUARES[square];
    uint64_t rank_mask = BB_RANKS[rank_of(square)];
    uint64_t file_mask = BB_FILES[file_of(square)];

    uint64_t rank_attacks = (((rank_mask & subset) - square_mask * 2) ^
                            get_reverse_bb(get_reverse_bb(rank_mask & subset) - get_reverse_bb(square_mask) * 2)) &
                            rank_mask;

    uint64_t file_attacks = (((file_mask & subset) - square_mask * 2) ^
                            get_reverse_bb(get_reverse_bb(file_mask & subset) - get_reverse_bb(square_mask) * 2)) &
                            file_mask;

    return rank_attacks | file_attacks;
}
