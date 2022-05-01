#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>


/**
 * Representation of the board using:
 * - bitboards for every color and piece type
 * - a bitboard of all occupied squares, and the occupied squares of just white/black
 * - a flag denoting whose turn it is
 * - flags for white/black castling rights kingside/queenside
 * - an en passant target square, if any
 * - a halfmove counter, denoting the number of halfmoves since the last capture or pawn advance
 * - a fullmove counter, denoting the number of cycles of a white move and a black move
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

    bool turn;

    bool w_kingside_castling_rights;
    bool w_queenside_castling_rights;
    bool b_kingside_castling_rights;
    bool b_queenside_castling_rights;

    int en_passant_square;

    int halfmove_clock;
    int fullmove_number;
} Board;


void init_board(Board *board, char *fen);


#endif
