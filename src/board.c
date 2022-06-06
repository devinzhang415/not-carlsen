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
 * Initalizes the board and other tables
 * @param board the uninitalized board structure
 * @param stack history of board positions and the moves it took to reach them
 * @param fen the FEN string to initalize the board to. Assumed valid
 */
void init(Board* board, Stack** stack, char* fen) {
    // Initalize board
    char fen_copy[100];
    strcpy(fen_copy, fen);
    char *rest = fen_copy;

    for (int i = A1; i <= H8; i++) {
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
                        set_bit(&board->w_pawns, square);
                        break;
                    case 'N':
                        set_bit(&board->w_knights, square);
                        break;
                    case 'B':
                        set_bit(&board->w_bishops, square);
                        break;
                    case 'R':
                        set_bit(&board->w_rooks, square);
                        break;
                    case 'Q':
                        set_bit(&board->w_queens, square);
                        break;
                    case 'K':
                        set_bit(&board->w_king, square);
                        break;
                    case 'p':
                        set_bit(&board->b_pawns, square);
                        break;
                    case 'n':
                        set_bit(&board->b_knights, square);
                        break;
                    case 'b':
                        set_bit(&board->b_bishops, square);
                        break;
                    case 'r':
                        set_bit(&board->b_rooks, square);
                        break;
                    case 'q':
                        set_bit(&board->b_queens, square);
                        break;
                    case 'k':
                        set_bit(&board->b_king, square);
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
    board->en_passant_square = (*token == '-') ? NULL_SQUARE : parse_square(token);

    token = strtok_r(rest, " ", &rest);
    board->halfmove_clock = atoi(token);

    token = strtok_r(rest, " ", &rest);
    board->fullmove_number = atoi(token);

    // Initalize stack
    Stack* node = malloc(sizeof(Stack));
    node->move = &NULL_MOVE;
    node->board = (*stack)->board;
    node->next = *stack;
    *stack = node;

    // Initalize tables
    init_rays(); // TODO unused?
    init_bishop_attacks();
    init_rook_attacks();
}


/**
 * Makes the given move and updates the stack
 * @param stack history of board positions and the moves it took to reach them
 * @param move
 */
void push(Stack** stack, Move* move) {
    Stack* node = malloc(sizeof(Stack));
    node->move = move;
    _make_move(&((*stack)->board), &move);
    node->board = (*stack)->board;
    node->next = *stack;
    *stack = node;
}


/**
 * Unmakes the most recent move and updates the stack
 * @param stack history of board positions and the moves it took to reach them
 */
void pop(Stack** stack) {
    Stack* temp = *stack;
    *stack = (*stack)->next;
    free(temp);
}


/**
 * Updates the board with the move
 * @param board
 * @param move 
 */
void _make_move(Board* board, Move* move) {
    // int from = move->from;
    // int to = move->from;
    // int flag = move->flag;
    // char attacker = board->mailbox[from];

    // if (flag == PASS) {
    //     board->turn = !board->turn;
    //     return;
    // }

    // bool reset_halfmove = false;
    // board->en_passant_square = NULL_SQUARE;

    // switch (attacker) {
    //     case 'P':
    //         clear_bit(&board->w_pawns, from);
    //         set_bit(&board->w_pawns, to);

    //         reset_halfmove = true;

    //         if (rank_of(to) - rank_of(from) == 2) board->en_passant_square = to - 8;

    //         if (flag == EN_PASSANT) {
    //             clear_bit(&board->b_pawns, to + 8);
    //         }

    //         break;
    //     case 'N':
    //         clear_bit(&board->w_knights, from);
    //         set_bit(&board->w_knights, to);
    //         break;
    //     case 'B':
    //         clear_bit(&board->w_bishops, from);
    //         set_bit(&board->w_bishops, to);
    //         break;
    //     case 'R':
    //         clear_bit(&board->w_rooks, from);
    //         set_bit(&board->w_rooks, to);

    //         if (from == H8) board->w_kingside_castling_rights = false;
    //         else if (from == A1) board->w_queenside_castling_rights = false;

    //         break;
    //     case 'Q':
    //         clear_bit(&board->w_queens, from);
    //         set_bit(&board->w_queens, to);
    //         break;
    //     case 'K':
    //         clear_bit(&board->w_king, from);
    //         set_bit(&board->w_king, to);

    //         if (flag == CASTLING) {
    //             if (file_of(to) - file_of(from) > 0) { // Kingside
    //                 clear_bit(&board->w_rooks, H1);
    //                 set_bit(&board->w_rooks, F1);
    //             } else { // Queenside
    //                 clear_bit(&board->w_rooks, A1);
    //                 set_bit(&board->w_rooks, D1);
    //             }
    //         }

    //         board->w_kingside_castling_rights = false;
    //         board->w_queenside_castling_rights = false;

    //         break;
    //     case 'p':
    //         clear_bit(&board->b_pawns, from);
    //         set_bit(&board->b_pawns, to);

    //         reset_halfmove = true;

    //         if (rank_of(to) - rank_of(from) == -2) board->en_passant_square = to + 8;

    //         if (flag == EN_PASSANT) {
    //             clear_bit(&board->w_pawns, to - 8);
    //         }

    //         break;
    //     case 'n':
    //         clear_bit(&board->b_knights, from);
    //         set_bit(&board->b_knights, to);
    //         break;
    //     case 'b':
    //         clear_bit(&board->b_bishops, from);
    //         set_bit(&board->b_bishops, to);
    //         break;
    //     case 'r':
    //         clear_bit(&board->b_rooks, from);
    //         set_bit(&board->b_rooks, to);

    //         if (from == H8) board->b_kingside_castling_rights = false;
    //         else if (from == A1) board->b_queenside_castling_rights = false;

    //         break;
    //     case 'q':
    //         clear_bit(&board->b_queens, from);
    //         set_bit(&board->b_queens, to);
    //         break;
    //     case 'k':
    //         clear_bit(&board->b_king, from);
    //         set_bit(&board->b_king, to);

    //         if (flag == CASTLING) {
    //             if (file_of(to) - file_of(from) > 0) { // Kingside
    //                 clear_bit(&board->b_rooks, H8);
    //                 set_bit(&board->b_rooks, F8);
    //             } else { // Queenside
    //                 clear_bit(&board->b_rooks, A8);
    //                 set_bit(&board->b_rooks, D8);
    //             }
    //         }

    //         board->b_kingside_castling_rights = false;
    //         board->b_queenside_castling_rights = false;

    //         break;
    // }

    // if (flag == CAPTURE) {
    //     _update_victim(board, to);
    //     reset_halfmove = true;
    // }

    // board->w_occupied = board->w_pawns | board->w_knights | board->w_bishops | board->w_rooks | board->w_queens | board->w_king;
    // board->b_occupied = board->b_pawns | board->b_knights | board->b_bishops | board->b_rooks | board->b_queens | board->b_king;
    // board->occupied = board->w_occupied | board->b_occupied;

    // board->turn = !board->turn;
    // if (reset_halfmove) {
    //     board->halfmove_clock = 0;
    // } else {
    //     board->halfmove_clock++;
    // }
    // board->fullmove_number++;
}


/**
 * Updates the taken piece's bitboard.
 * @param board 
 * @param target the square the victim is on 
 */
void _update_victim(Board* board, int target) {
    char victim = board->mailbox[target];

    switch (victim) {
        case 'P':
            clear_bit(&board->w_pawns, target);
            break;
        case 'N':
            clear_bit(&board->w_knights, target);
            break;
        case 'B':
            clear_bit(&board->w_bishops, target);
            break;
        case 'R':
            clear_bit(&board->w_rooks, target);
            break;
        case 'Q':
            clear_bit(&board->w_queens, target);
            break;
        case 'K':
            clear_bit(&board->w_king, target);
            break;
        case 'p':
            clear_bit(&board->b_pawns, target);
            break;
        case 'n':
            clear_bit(&board->b_knights, target);
            break;
        case 'b':
            clear_bit(&board->b_bishops, target);
            break;
        case 'r':
            clear_bit(&board->b_rooks, target);
            break;
        case 'q':
            clear_bit(&board->b_queens, target);
            break;
        case 'k':
            clear_bit(&board->b_king, target);
            break;
    }
}
