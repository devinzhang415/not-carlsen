#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "board.h"
#include "util.h"
#include "movegen.h"


/**
 * @brief Initalizes the board and sliding piece attack tables
 * @param board the uninitalized board structure
 * @param fen the FEN string to initalize the board to. Assumed valid
 */
void init_board(Board *board, char *fen) {
    char fen_copy[100];
    strcpy(fen_copy, fen);
    char *rest = fen_copy;

    for (int i = A1; i < H8; i++) {
        board->mailbox[i] = '-';
    }
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

    char *token = strtok_r(rest, " ", &rest);
    for (int rank = 7; rank >= 0; rank--) {
        char *fen_board = strtok_r(token, "/", &token);
        int file = 0;
        for (int j = 0; j < strlen(fen_board); j++) {
            if (file >= 8) break;

            char piece = fen_board[j];
            if (isdigit(piece)) {
                file += piece - '0';
            } else {
                int square = 8*rank + file;
                board->mailbox[square] = piece;
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
    board->w_occupied = board->w_pawns | board->w_knights | board->w_bishops | board->w_rooks | board->w_queens | board->w_king;
    board->b_occupied = board->b_pawns | board->b_knights | board->b_bishops | board->b_rooks | board->b_queens | board->b_king;
    board->occupied = board->w_occupied | board->b_occupied;

    token = strtok_r(rest, " ", &rest);
    board->turn = (*token == 'w') ? WHITE : BLACK;

    token = strtok_r(rest, " ", &rest);
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

    token = strtok_r(rest, " ", &rest);
    board->en_passant_square = (*token == '-') ? -1 : parse_square(token);

    token = strtok_r(rest, " ", &rest);
    board->halfmove_clock = atoi(token);

    token = strtok_r(rest, " ", &rest);
    board->fullmove_number = atoi(token);


    init_rays();
    init_bishop_attacks();
    init_rook_attacks();
}
