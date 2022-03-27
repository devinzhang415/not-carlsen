#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
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
 * Parses a string square (eg, "a1") to an int square.
 * 
 * @param square the square name given in string format.
 * @return the square's integer value.
 */
int parse_square(char *square) {
    int file = square[0] - 'a';
    int rank = square[1] - '0';
    return 8 * (rank - 1) + file;
}

/**
 * Initalizes the board to the given FEN. Assumes the FEN is valid.
 * 
 * @param board the uninitalized board.
 * @param fen the FEN string to initalize the board to.
 */
void init_board(Board *board, char *fen) {
    char fen_copy[100];
    strcpy(fen_copy, fen);
    char *rest = fen_copy;

    for (int i = 0; i < 6; i++) {
        char *token = strtok_r(rest, " ", &rest);
        switch (i) {
            case 0:
                board->w_pawns = 0;
                board->w_knights = 0;
                board->w_bishops = 0;
                board->w_rooks = 0;
                board->w_queens = 0;
                board->w_king = 0;
                board->b_pawns = 0;
                board->b_knights = 0;
                board->b_bishops = 0;
                board->b_rooks = 0;
                board->b_queens = 0;
                board->b_king = 0;
                board->occupied = 0;
                board->w_occupied = 0;
                board->b_occupied = 0;

                for (int rank = 7; rank >= 0; rank--) {
                    char *fen_board = strtok_r(token, "/", &token);
                    int file = 0;
                    for (int j = 0; j < strlen(fen_board); j++) {
                        if (file >= 8) {
                            break;
                        }
                        char piece = fen_board[j];
                        if (isdigit(piece)) {
                            file += piece - '0';
                        } else {
                            int square = 8*rank + file;

                            board->occupied |= 1ULL << square;
                            if (isupper(piece)) {
                                board->w_occupied |= 1ULL << square;
                            } else {
                                board->b_occupied |= 1ULL << square;
                            }

                            switch (piece) {
                                case 'P':
                                    board->w_pawns |= 1ULL << square;
                                    break;
                                case 'N':
                                    board->w_knights |= 1ULL << square;
                                    break;
                                case 'B':
                                    board->w_bishops |= 1ULL << square;
                                    break;
                                case 'R':
                                    board->w_rooks |= 1ULL << square;
                                    break;
                                case 'Q':
                                    board->w_queens |= 1ULL << square;
                                    break;
                                case 'K':
                                    board->w_king |= 1ULL << square;
                                    break;
                                case 'p':
                                    board->b_pawns |= 1ULL << square;
                                    break;
                                case 'n':
                                    board->b_knights |= 1ULL << square;
                                    break;
                                case 'b':
                                    board->b_bishops |= 1ULL << square;
                                    break;
                                case 'r':
                                    board->b_rooks |= 1ULL << square;
                                    break;
                                case 'q':
                                    board->b_queens |= 1ULL << square;
                                    break;
                                case 'k':
                                    board->b_king |= 1ULL << square;
                                    break;
                            }
                            file++;
                        }
                    }
                }
                break;
            case 1:
                board->turn = (*token == 'w') ? WHITE : BLACK;
                break;
            case 2:
                board->w_kingside_castling_rights = false;
                board->w_queenside_castling_rights = false;
                board->b_kingside_castling_rights = false;
                board->b_queenside_castling_rights = false;
                for (int i = 0; i < strlen(token); i++) {
                    char piece = token[i];
                    switch (piece) {
                        case 'K':
                            board->w_kingside_castling_rights = true;
                            break;
                        case 'Q':
                            board->w_queenside_castling_rights = true;
                            break;
                        case 'k':
                            board->b_kingside_castling_rights = true;
                            break;
                        case 'q':
                            board->b_queenside_castling_rights = true;
                            break;
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

/**
 * Gets the bit from the bitboard at the given square.
 * 
 * @param bb the bitboard to get the square from.
 * @param square the square to get the bit from.
 * @return the bit at the square (0 or 1).
 */
bool get_bit(uint64_t bb, int square) {
    return bb & (1ULL << square);
}

/**
 * Prints the binary representation of the bitboard.
 * 
 * @param bb the bitboard to be printed.
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

int main() {
    Board board;

    // init_board(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    init_board(&board, "r1bqk2r/pppp1ppp/2n2n2/8/1bPP4/2N2N2/PP3PPP/R1BQKB1R w KQkq - 0 1");

    // print_bb(board.w_pawns);
    // print_bb(board.w_knights);
    // print_bb(board.w_bishops);
    // print_bb(board.w_rooks);
    // print_bb(board.w_queens);
    // print_bb(board.w_king);
    // print_bb(board.b_pawns);
    // print_bb(board.b_knights);
    // print_bb(board.b_bishops);
    // print_bb(board.b_rooks);
    // print_bb(board.b_queens);
    // print_bb(board.b_king);

    // print_bb(board.occupied);
    // print_bb(board.w_occupied);
    // print_bb(board.b_occupied);

    // printf("Turn: %d\n%d%d%d%d\nEn passant: %d\nHalfmove: %d\nFullmove: %d\n", board.turn,
    //                                                                            board.w_kingside_castling_rights, board.w_queenside_castling_rights, board.b_kingside_castling_rights, board.b_queenside_castling_rights,
    //                                                                            board.en_passant_square,
    //                                                                            board.halfmove_clock,
    //                                                                            board.fullmove_number);

    return 0;
}