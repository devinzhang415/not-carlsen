#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "constants.h"

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

/**
 * Parses a string square to an int square.
 * 
 * @param square the square name given in string format.
 * @return the square's integer value.
 */
int parse_square(char* square) {
    int file = square[0] - 'a';
    int rank = square[1] - '0';
    return 8 * (rank - 1) + (file);
}

/**
 * Initalizes the board to the given FEN. Assumes the FEN is valid.
 * 
 * @param board the uninitalized board.
 * @param fen the FEN string to initalize the board to.
 */
void init_board(Board* board, char* fen) {
    char fen_copy[100];
    strcpy(fen_copy, fen);

    char* token;
    for (int i = 0; i < 6; i++) {
        if (i == 0) {
            token = strtok(fen_copy, " ");
        } else {
            token = strtok(NULL, " ");
            switch (i) {
                case 1:
                    board->turn = (*token == 'w') ? WHITE : BLACK;
                    break;
                case 2:
                    board->w_kingside_castling_rights = false;
                    board->w_queenside_castling_rights = false;
                    board->b_kingside_castling_rights = false;
                    board->b_queenside_castling_rights = false;
                    for (int i = 0; i < strlen(token); i++) {
                        if (token[i] == 'K') {
                            board->w_kingside_castling_rights = true;
                        } else if (token[i] == 'Q') {
                            board->w_queenside_castling_rights = true;
                        } else if (token[i] == 'k') {
                            board->b_kingside_castling_rights = true;
                        } else if (token[i] == 'q') {
                            board->b_queenside_castling_rights = true;
                        }
                    }
                    break;
                case 3:
                    board->en_passant_square = (*token == '-') ? -1 : parse_square(token);
                    break;
                case 4:
                    board->halfmove_clock = atoi(token);
                    break;
                case 5:
                    board->fullmove_number = atoi(token);
                    break;
            }
        }
    }
    // board->w_rooks = 1ULL << (63 - A1);
}

int main() {
    Board board;

    init_board(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // printf("%llu\n", board.w_rooks);
    printf("%d\n%d%d%d%d\n%d\n%d\n%d\n", board.turn,
                                         board.w_kingside_castling_rights, board.w_queenside_castling_rights, board.b_kingside_castling_rights, board.b_queenside_castling_rights,
                                         board.en_passant_square,
                                         board.halfmove_clock,
                                         board.fullmove_number);

    return 0;
}