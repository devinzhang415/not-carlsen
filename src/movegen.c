#include <stdio.h>
#include <stdint.h>
#include <ctype.h> 
#include <stdlib.h>
#include <time.h>
#include "movegen.h"
#include "util.h"
#include "board.h"
#include "stack.h"


extern Board board;
extern Stack* stack;
extern RTable rtable;


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
 * Initalizes the bishop attack magic bitboard
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
 * Initalizes the rook attack magic bitboard
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
 * Helper method to initalizes the bishop attack magic bitboard
 * @param square the current square
 * @param subset the current occupancy
 * @param attack_mask the bishop's attack mask without edges
 * @return the bishop attack bitboard
 * @author github.com/nkarve
 */
static uint64_t _init_bishop_attacks_helper(int square, uint64_t subset) {
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
 * Helper method to initalizes the rook attack magic bitboard
 * @param square the current square
 * @param subset the current occupancy
 * @param attack_mask the rook's attack mask without edges
 * @return the rook attack bitboard
 * @author github.com/nkarve
 */
static uint64_t _init_rook_attacks_helper(int square, uint64_t subset) {
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


/**
 * Prints out the legal perft grouped by the first moves made.
 * @param depth what depth to perform moves to.
 * @return the number of legal moves at depth n.
 * 
 * https://www.chessprogramming.org/Perft_Results
 * pos 1 accurate to depth 7
 * pos 2 accurate to depth 5
 * pos 3 accurate to depth 8
 * pos 4 accurate to depth 6
 * pos 5 accurate to depth 5
 * pos 6 accurate to depth 5
 */
uint64_t print_divided_legal_perft(int depth) {
    clock_t start = clock();

    uint64_t total_nodes = 0;
    Move moves[MAX_MOVE_NUM];

    gen_legal_moves(moves, board.turn);
    for (int i = 0; i < MAX_MOVE_NUM; i++) {
        if (moves[i].flag == INVALID) break;

        push(moves[i]);
        if (is_game_over()) break;

        print_move_post(moves[i]);
        uint64_t nodes = legal_perft(depth - 1);
        total_nodes += nodes;
        printf(": %llu\n", nodes);
        pop();
    }
    printf("\nNodes searched: %llu\n", total_nodes);

    clock_t elapsed = clock() - start;
    double time = (double) elapsed / CLOCKS_PER_SEC;
    printf("nps: %.0f\n\n", total_nodes / time);

    return total_nodes;
}


/**
 * Performance test debug function to determine the accuracy of the legal move generator.
 * @return the number of legal moves at depth n.
 */
static uint64_t legal_perft(int depth) {
    uint64_t nodes = 0;

    if (depth == 0) return 1ULL;

    Move moves[MAX_MOVE_NUM];
    gen_legal_moves(moves, board.turn);

    for (int i = 0; i < MAX_MOVE_NUM; i++) {
        if (moves[i].flag == INVALID) break;

        push(moves[i]);
        if (is_game_over()) break;

        nodes += legal_perft(depth - 1);
        pop();
    }
    return nodes;
}


/**
 * Prints out the pseudolegal perft grouped by the first moves made.
 * @param depth what depth to perform moves to.
 * @return the number of legal moves at depth n.
 * 
 * https://www.chessprogramming.org/Perft_Results
 * pos 1 accurate to depth 7
 * pos 2 fails on depth 5
 * pos 3 fails on depth 7
 * pos 4 fails on depth 4
 * pos 5 fails on depth 5
 * pos 6 accurate to depth 5
 */
uint64_t print_divided_pseudolegal_perft(int depth) {
    clock_t start = clock();

    uint64_t total_nodes = 0;
    Move moves[MAX_MOVE_NUM];

    gen_pseudolegal_moves(moves, board.turn);
    for (int i = 0; i < MAX_MOVE_NUM; i++) {
        if (moves[i].flag == INVALID) break;

        if (!legal_push(moves[i])) continue;
        if (is_game_over()) break;

        print_move_post(moves[i]);
        uint64_t nodes = pseudolegal_perft(depth - 1);
        total_nodes += nodes;
        printf(": %llu\n", nodes);
        pop();
    }
    printf("\nNodes searched: %llu\n", total_nodes);

    clock_t elapsed = clock() - start;
    double time = (double) elapsed / CLOCKS_PER_SEC;
    printf("nps: %.0f\n\n", total_nodes / time);

    return total_nodes;
}


/**
 * Performance test debug function to determine the accuracy of the pseudolegal move generator.
 * @param depth what depth to perform moves to.
 * @return the number of legal moves at depth n.
 */
static uint64_t pseudolegal_perft(int depth) {
    uint64_t nodes = 0;

    if (depth == 0) return 1ULL;

    Move moves[MAX_MOVE_NUM];
    gen_pseudolegal_moves(moves, board.turn);

    for (int i = 0; i < MAX_MOVE_NUM; i++) {
        if (moves[i].flag == INVALID) break;

        if (!legal_push(moves[i])) continue;
        if (is_game_over()) break;

        nodes += pseudolegal_perft(depth - 1);
        pop();
    }
    return nodes;
}


/**
 * Takes in an empty array and generates the list of legal moves in it.
 * @param moves the array to store the moves in.
 * @param color the side to move.
 */
void gen_legal_moves(Move* moves, bool color) {
    int i = 0;

    uint64_t pieces;
    uint64_t king_bb;
    char piece;
    uint64_t enemy_pawns_attacks;
    uint64_t enemy_rq_bb;
    uint64_t enemy_bq_bb;
    if (color == WHITE) {
        pieces = board.w_occupied;
        king_bb = board.w_king;
        piece = 'K';
        enemy_pawns_attacks = (((board.b_pawns >> 9) & ~BB_FILE_H) | ((board.b_pawns >> 7) & ~BB_FILE_A))
                               & board.w_occupied;
        enemy_rq_bb = board.b_rooks | board.b_queens;
        enemy_bq_bb = board.b_bishops | board.b_queens;
    } else {
        pieces = board.b_occupied;
        king_bb = board.b_king;
        piece = 'k';
        enemy_pawns_attacks = (((board.w_pawns << 9) & ~BB_FILE_A) | ((board.w_pawns << 7) & ~BB_FILE_H))
                               & board.b_occupied;
        enemy_rq_bb = board.w_rooks | board.w_queens;
        enemy_bq_bb = board.w_bishops | board.w_queens;
    }
    int king_square = get_lsb(king_bb);

    uint64_t attackmask = _get_attackmask(!color);
    uint64_t checkmask = _get_checkmask(color);
    uint64_t pos_pinned = get_queen_moves(!color, king_square);

    // King is in double check, only moves are to move king away
    if (!checkmask) {
        uint64_t moves_bb = get_king_moves(color, king_square) & ~_get_attackmask(!color);
        while (moves_bb) {
            int to = pull_lsb(&moves_bb);
            int flag = _get_flag(color, piece, king_square, to);
            if (flag == CASTLING) continue;
            Move move = {king_square, to, flag};
            moves[i++] = move;
        }
        Move end = {A1, A1, INVALID};
        moves[i] = end;
        return;
    }

    while (pieces) {
        int from = pull_lsb(&pieces);
        char piece = board.mailbox[from];

        uint64_t pinmask;
        if (BB_SQUARES[from] & pos_pinned) {
            pinmask = _get_pinmask(color, from);
        } else {
            pinmask = BB_ALL;
        }

        uint64_t moves_bb;
        switch (toupper(piece)) {
            case 'P':
                uint64_t pawn_moves = get_pawn_moves(color, from);
                moves_bb = pawn_moves & checkmask & pinmask;

                if (board.en_passant_square != NULL_SQUARE) {
                    if (pawn_moves & pinmask & BB_SQUARES[board.en_passant_square]) {
                        // Add possible en passant capture to remove check
                        // For example en passant is legal here:
                        // 8/8/8/2k5/3Pp3/8/8/3K4 b - d3 0 1
                        if (king_bb & enemy_pawns_attacks) {
                            set_bit(&moves_bb, board.en_passant_square);
                        }
                    }
                }

                break;
            case 'N':
                moves_bb = get_knight_moves(color, from) & checkmask & pinmask;
                break;
            case 'B':
                moves_bb = get_bishop_moves(color, from) & checkmask & pinmask;
                break;
            case 'R':
                moves_bb = get_rook_moves(color, from) & checkmask & pinmask;
                break;
            case 'Q':
                moves_bb = get_queen_moves(color, from) & checkmask & pinmask;
                break;
            case 'K':
                moves_bb = get_king_moves(color, from) & ~attackmask;
                break;
        }

        while (moves_bb) {
            int to = pull_lsb(&moves_bb);

            if (toupper(piece) == 'P' && (rank_of(to) == 0 || rank_of(to) == 7)) { // Add all promotions
                Move queen_promotion = {from, to, PROMOTION_QUEEN};
                moves[i++] = queen_promotion;
                Move rook_promotion = {from, to, PROMOTION_ROOK};
                moves[i++] = rook_promotion;
                Move bishop_promotion = {from, to, PROMOTION_BISHOP};
                moves[i++] = bishop_promotion;
                Move knight_promotion = {from, to, PROMOTION_KNIGHT};
                moves[i++] = knight_promotion;
            } else {
                int flag = _get_flag(color, piece, from, to);
                Move move = {from, to, flag};

                // Determine if castling is legal
                if (flag == CASTLING) {
                    if (attackmask & king_bb) continue; // Assert the king is not in check
                    if (board.turn == WHITE) {
                        if (from != E1) continue; // Assert the king is still alive
                        if (to == G1) { // Kingside
                            if (!board.w_kingside_castling_rights) continue; // Assert king or rook has not moved
                            if (!(board.w_rooks & BB_SQUARES[H1])) continue; // Assert rook is still alive
                            if (board.occupied & (BB_SQUARES[F1] | BB_SQUARES[G1])) continue; // Assert there are no pieces between the king and rook
                            if (attackmask & (BB_SQUARES[F1] | BB_SQUARES[G1])) continue; // Assert the squares the king moves through are not attacked
                        } else if (to == C1) { // Queenside
                            if (!board.w_queenside_castling_rights) continue;
                            if (!(board.w_rooks & BB_SQUARES[A1])) continue;
                            if (board.occupied & (BB_SQUARES[D1] | BB_SQUARES[C1] | BB_SQUARES[B1])) continue;
                            if (attackmask & (BB_SQUARES[D1] | BB_SQUARES[C1])) continue;
                        } else {
                            continue;
                        }
                    } else {
                        if (from != E8) continue;
                        if (to == G8) { // Kingside
                            if (!board.b_kingside_castling_rights) continue;
                            if (!(board.b_rooks & BB_SQUARES[H8])) continue;
                            if (board.occupied & (BB_SQUARES[F8] | BB_SQUARES[G8])) continue;
                            if (attackmask & (BB_SQUARES[F8] | BB_SQUARES[G8])) continue;
                        } else if (to == C8) { // Queenside
                            if (!board.b_queenside_castling_rights) continue;
                            if (!(board.b_rooks & BB_SQUARES[A8])) continue;
                            if (board.occupied & (BB_SQUARES[D8] | BB_SQUARES[C8] | BB_SQUARES[B8])) continue;
                            if (attackmask & (BB_SQUARES[D8] | BB_SQUARES[C8])) continue;
                        } else {
                            continue;
                        }
                    }
                } else if (flag == EN_PASSANT) {
                    // Remove possible en passant capture that leaves king in check
                    // For example en passant is illegal here:
                    // 8/8/8/8/k2Pp2Q/8/8/3K4 b - d3 0 1
                    // k7/1q6/8/3pP3/8/5K2/8/8 w - d6 0 1
                    push(move);
                    bool invalid = is_check(color);
                    pop();
                    if (invalid) continue;
                }

                moves[i++] = move;
            }
        }
    }
    Move end = {A1, A1, INVALID}; // Flag move list end
    moves[i] = end;
}


/**
 * Takes in an empty array and generates the list of pseudolegal moves in it.
 * @param moves the array to store the moves in.
 * @param color the side to move.
 */
void gen_pseudolegal_moves(Move* moves, bool color) {
    int i = 0;

    uint64_t pieces = (color == WHITE) ? board.w_occupied : board.b_occupied;
    while (pieces) {
        int from = pull_lsb(&pieces);
        char piece = board.mailbox[from];

        uint64_t moves_bb;
        switch (toupper(piece)) {
            case 'P':
                moves_bb = get_pawn_moves(color, from);
                break;
            case 'N':
                moves_bb = get_knight_moves(color, from);
                break;
            case 'B':
                moves_bb = get_bishop_moves(color, from);
                break;
            case 'R':
                moves_bb = get_rook_moves(color, from);
                break;
            case 'Q':
                moves_bb = get_queen_moves(color, from);
                break;
            case 'K':
                moves_bb = get_king_moves(color, from);
                break;
        }

        while (moves_bb) {
            int to = pull_lsb(&moves_bb);
            if (toupper(piece) == 'P' && (rank_of(to) == 0 || rank_of(to) == 7)) { // Add all promotions
                Move queen_promotion = {from, to, PROMOTION_QUEEN};
                moves[i++] = queen_promotion;
                Move rook_promotion = {from, to, PROMOTION_ROOK};
                moves[i++] = rook_promotion;
                Move bishop_promotion = {from, to, PROMOTION_BISHOP};
                moves[i++] = bishop_promotion;
                Move knight_promotion = {from, to, PROMOTION_KNIGHT};
                moves[i++] = knight_promotion;
            } else {
                int flag = _get_flag(color, piece, from, to);
                Move move = {from, to, flag};
                moves[i++] = move;
            }
        }
    }
    Move end = {A1, A1, INVALID};
    moves[i] = end;
}


/**
 * @param color the side to move
 * @param piece
 * @param from the square the piece is moving from
 * @param to the square the piece is moving to 
 * @return the appropriate flag for the move, excludes promotions
 */
static int _get_flag(bool color, char piece, int from, int to) {
    switch (toupper(piece)) {
        case 'P':
            if (to == board.en_passant_square) return EN_PASSANT;
        case 'K':
            if (abs(file_of(from) - file_of(to)) == 2) return CASTLING;
    }
    if (BB_SQUARES[to] & board.occupied) return CAPTURE;
    return NONE;
}


/**
 * @param color 
 * @return the bitboard of squares the king of the color can't go.
 * All squares the color is attacking.
 */
static uint64_t _get_attackmask(bool color) {
    uint64_t occupied;
    uint64_t key;

    uint64_t moves_bb;
    uint64_t pieces;
    int king_square;
    if (color == WHITE) {
        pieces = board.w_occupied ^ board.w_pawns;
        king_square = get_lsb(board.b_king);
        moves_bb = (((board.w_pawns << 9) & ~BB_FILE_A) | ((board.w_pawns << 7) & ~BB_FILE_H));
    } else {
        pieces = board.b_occupied ^ board.b_pawns;
        king_square = get_lsb(board.w_king);
        moves_bb = (((board.b_pawns >> 9) & ~BB_FILE_H) | ((board.b_pawns >> 7) & ~BB_FILE_A));
    }

    clear_bit(&board.occupied, king_square);

    while (pieces) {
        int square = pull_lsb(&pieces);
        char piece = board.mailbox[square];
        switch (toupper(piece)) {
            case 'N':
                moves_bb |= BB_KNIGHT_ATTACKS[square];
                break;
            case 'B':
                occupied = board.occupied & BB_BISHOP_ATTACK_MASKS[square];
                key = (occupied * BISHOP_MAGICS[square]) >> BISHOP_ATTACK_SHIFTS[square];
                moves_bb |= BB_BISHOP_ATTACKS[square][key];
                break;
            case 'R':
                occupied = board.occupied & BB_ROOK_ATTACK_MASKS[square];
                key = (occupied * ROOK_MAGICS[square]) >> ROOK_ATTACK_SHIFTS[square];
                moves_bb |= BB_ROOK_ATTACKS[square][key];
                break;
            case 'Q':
                occupied = board.occupied & BB_BISHOP_ATTACK_MASKS[square];
                key = (occupied * BISHOP_MAGICS[square]) >> BISHOP_ATTACK_SHIFTS[square];
                moves_bb |= BB_BISHOP_ATTACKS[square][key];

                occupied = board.occupied & BB_ROOK_ATTACK_MASKS[square];
                key = (occupied * ROOK_MAGICS[square]) >> ROOK_ATTACK_SHIFTS[square];
                moves_bb |= BB_ROOK_ATTACKS[square][key];
                break;
            case 'K':
                moves_bb |= BB_KING_ATTACKS[square];
                break;
        }
    }

    set_bit(&board.occupied, king_square);

    return moves_bb;
}


/**
 * @param color the color of the king possibly in check.
 * @return all squares if the king is not in check, else
 * the path between the attacking piece and the color's king. 
 * Returns empty bitboard if in double or more check.
 */
static uint64_t _get_checkmask(bool color) {
    int num_attackers = 0;
    uint64_t checkmask = 0;

    int king_square;
    uint64_t enemy_queen_bb;
    uint64_t enemy_rook_bb;
    uint64_t enemy_bishop_bb;
    uint64_t enemy_knight_bb;
    uint64_t pawns;
    if (color == WHITE) {
        king_square = get_lsb(board.w_king);
        enemy_queen_bb = board.b_queens;
        enemy_rook_bb = board.b_rooks;
        enemy_bishop_bb = board.b_bishops;
        enemy_knight_bb = board.b_knights;
        pawns = ((((board.w_king << 9) & ~BB_FILE_A) | ((board.w_king << 7) & ~BB_FILE_H))
                  & board.b_pawns);
    } else {
        king_square = get_lsb(board.b_king);
        enemy_queen_bb = board.w_queens;
        enemy_rook_bb = board.w_rooks;
        enemy_bishop_bb = board.w_bishops;
        enemy_knight_bb = board.w_knights;
        pawns = ((((board.b_king >> 9) & ~BB_FILE_H) | ((board.b_king >> 7) & ~BB_FILE_A))
                  & board.w_pawns);
    }

    num_attackers += pop_count(pawns);
    if (num_attackers >= 2) {
        return 0;
    }
    checkmask |= pawns;

    uint64_t queens = get_queen_moves(color, king_square) & enemy_queen_bb;
    num_attackers += pop_count(queens);
    if (num_attackers >= 2) {
        return 0;
    }
    while (queens) {
        int queen_square = pull_lsb(&queens);
        checkmask |= get_ray_between(king_square, queen_square);
    }

    uint64_t rooks = get_rook_moves(color, king_square) & enemy_rook_bb;
    num_attackers += pop_count(rooks);
    if (num_attackers >= 2) {
        return 0;
    }
    while (rooks) {
        int rook_square = pull_lsb(&rooks);
        checkmask |= get_ray_between(king_square, rook_square);
    }

    uint64_t bishops = get_bishop_moves(color, king_square) & enemy_bishop_bb;
    num_attackers += pop_count(bishops);
    if (num_attackers >= 2) {
        return 0;
    }
    while (bishops) {
        int bishop_square = pull_lsb(&bishops);
        checkmask |= get_ray_between(king_square, bishop_square);
    }

    uint64_t knights = get_knight_moves(color, king_square) & enemy_knight_bb;
    num_attackers += pop_count(knights);
    if (num_attackers >= 2) {
        return 0;
    }
    checkmask |= knights;
    
    if (num_attackers == 0) return BB_ALL;
    return checkmask;
}


/**
 * @param color
 * @param square the square the possibly pinned piece is on.
 * @return a possible pin ray for the piece.
 */
static uint64_t _get_pinmask(bool color, int square) {
    uint64_t pinmask = 0;

    uint64_t king_bb;
    uint64_t enemy_rq_bb;
    uint64_t enemy_bq_bb;
    if (color == WHITE) {
        king_bb = board.w_king;
        enemy_rq_bb = board.b_rooks | board.b_queens;
        enemy_bq_bb = board.b_bishops | board.b_queens;
    } else {
        king_bb = board.b_king;
        enemy_rq_bb = board.w_rooks | board.w_queens;
        enemy_bq_bb = board.w_bishops | board.w_queens;
    }

    uint64_t occupied = board.occupied & BB_ROOK_ATTACK_MASKS[square];
    uint64_t key = (occupied * ROOK_MAGICS[square]) >> ROOK_ATTACK_SHIFTS[square];
    uint64_t rook_attacks = BB_ROOK_ATTACKS[square][key];

    occupied = board.occupied & BB_BISHOP_ATTACK_MASKS[square];
    key = (occupied * BISHOP_MAGICS[square]) >> BISHOP_ATTACK_SHIFTS[square];
    uint64_t bishop_attacks = BB_BISHOP_ATTACKS[square][key];

    uint64_t rank = BB_RANKS[rank_of(square)] & rook_attacks;
    if (rank & king_bb && rank & enemy_rq_bb) {
        pinmask |= rank;
    }

    uint64_t file = BB_FILES[file_of(square)] & rook_attacks;
    if (file & king_bb && file & enemy_rq_bb) {
        pinmask |= file;
    }

    uint64_t diagonal = BB_DIAGONALS[diagonal_of(square)] & bishop_attacks;
    if (diagonal & king_bb && diagonal & enemy_bq_bb) {
        pinmask |= diagonal;
    }

    uint64_t anti_diagonal = BB_ANTI_DIAGONALS[anti_diagonal_of(square)] & bishop_attacks;
    if (anti_diagonal & king_bb && anti_diagonal & enemy_bq_bb) {
        pinmask |= anti_diagonal;
    }
    
    if (!pinmask) return BB_ALL;
    return pinmask;
}


/**
 * @param color the color of the pawn.
 * @param square the square the pawn is on.
 * @return where the pawn can move from the given square.
 */
uint64_t get_pawn_moves(bool color, int square) {
    if (color == WHITE) {
        uint64_t pawn = BB_SQUARES[square];

        uint64_t single_push = (pawn << 8) & ~board.occupied;
        uint64_t double_push = ((single_push & BB_RANK_3) << 8) & ~board.occupied;

        uint64_t captures = (((pawn << 9) & ~BB_FILE_A) | ((pawn << 7) & ~BB_FILE_H))
                            & board.b_occupied;

        if (board.en_passant_square != NULL_SQUARE && rank_of(square) + 1 == 5) {
            uint64_t ep_capture = (((pawn << 9) & ~BB_FILE_A) | ((pawn << 7) & ~BB_FILE_H))
                                   & BB_SQUARES[board.en_passant_square];
            captures |= ep_capture;
        }

        return single_push | double_push | captures;
    } else {
        uint64_t pawn = BB_SQUARES[square];

        uint64_t single_push = (pawn >> 8) & ~board.occupied;
        uint64_t double_push = ((single_push & BB_RANK_6) >> 8) & ~board.occupied;

        uint64_t captures = (((pawn >> 9) & ~BB_FILE_H) | ((pawn >> 7) & ~BB_FILE_A))
                            & board.w_occupied;

        if (board.en_passant_square != NULL_SQUARE && rank_of(square) + 1 == 4) {
            uint64_t ep_capture = (((pawn >> 9) & ~BB_FILE_H) | ((pawn >> 7) & ~BB_FILE_A))
                                   & BB_SQUARES[board.en_passant_square];
            captures |= ep_capture;
        }

        return single_push | double_push | captures;
    }
}


/**
 * @param color the color of the knight
 * @param square the square the knight is on
 * @return where the knight can move from the given square
 */
uint64_t get_knight_moves(bool color, int square) {
    uint64_t moves = BB_KNIGHT_ATTACKS[square];

    return (color == WHITE) ? moves & ~board.w_occupied : moves & ~board.b_occupied;
}


/**
 * @param color the color of the bishop
 * @param square the square the bishop is on
 * @return where the bishop can move from the given square
 */
uint64_t get_bishop_moves(bool color, int square) {
    uint64_t occupied = board.occupied & BB_BISHOP_ATTACK_MASKS[square];
    uint64_t key = (occupied * BISHOP_MAGICS[square]) >> BISHOP_ATTACK_SHIFTS[square];
    uint64_t moves = BB_BISHOP_ATTACKS[square][key];

    return (color == WHITE) ? moves & ~board.w_occupied : moves & ~board.b_occupied;
}


/**
 * @param color the color of the rook
 * @param square the square the rook is on
 * @return where the rook can move from the given square
 */
uint64_t get_rook_moves(bool color, int square) {
    uint64_t occupied = board.occupied & BB_ROOK_ATTACK_MASKS[square];
    uint64_t key = (occupied * ROOK_MAGICS[square]) >> ROOK_ATTACK_SHIFTS[square];
    uint64_t moves = BB_ROOK_ATTACKS[square][key];

    return (color == WHITE) ? moves & ~board.w_occupied : moves & ~board.b_occupied;
}


/**
 * @param color the color of the queen
 * @param square the square the queen is on
 * @return where the queen can move from the given square
 */
uint64_t get_queen_moves(bool color, int square) {
    uint64_t bishop_occupied = board.occupied & BB_BISHOP_ATTACK_MASKS[square];
    uint64_t bishop_key = (bishop_occupied * BISHOP_MAGICS[square]) >> BISHOP_ATTACK_SHIFTS[square];
    uint64_t bishop_moves = BB_BISHOP_ATTACKS[square][bishop_key];

    uint64_t rook_occupied = board.occupied & BB_ROOK_ATTACK_MASKS[square];
    uint64_t rook_key = (rook_occupied * ROOK_MAGICS[square]) >> ROOK_ATTACK_SHIFTS[square];
    uint64_t rook_moves = BB_ROOK_ATTACKS[square][rook_key];

    uint64_t moves = bishop_moves | rook_moves;

    return (color == WHITE) ? moves & ~board.w_occupied : moves & ~board.b_occupied;
}


/**
 * @param color the color of the king
 * @param square the square the king is on
 * @return where the king can move from the given square
 */
uint64_t get_king_moves(bool color, int square) {
    uint64_t moves = BB_KING_ATTACKS[square];
    if (color == WHITE) {
        if (board.w_kingside_castling_rights) set_bit(&moves, G1);
        if (board.w_queenside_castling_rights) set_bit(&moves, C1);
        return moves & ~board.w_occupied;
    } else {
        if (board.b_kingside_castling_rights) set_bit(&moves, G8);
        if (board.b_queenside_castling_rights) set_bit(&moves, C8);
        return moves & ~board.b_occupied;
    }
}
