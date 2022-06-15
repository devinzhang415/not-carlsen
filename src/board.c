#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
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
    // Initalize misc
    srand(time(NULL)); 
    init_zobrist_table();
    init_rays(); // TODO unused?
    init_bishop_attacks();
    init_rook_attacks();

    char fen_copy[100];
    strcpy(fen_copy, fen);
    char *rest = fen_copy;

    // Initalize bitboards and mailbox
    char *token = strtok_r(rest, " ", &rest);
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
    board->bitboards[0] = &board->w_pawns;
    board->bitboards[1] = &board->w_knights;
    board->bitboards[2] = &board->w_bishops;
    board->bitboards[3] = &board->w_rooks;
    board->bitboards[4] = &board->w_queens;
    board->bitboards[5] = &board->w_king;
    board->bitboards[6] = &board->b_pawns;
    board->bitboards[7] = &board->b_knights;
    board->bitboards[8] = &board->b_bishops;
    board->bitboards[9] = &board->b_rooks;
    board->bitboards[10] = &board->b_queens;
    board->bitboards[11] = &board->b_king;
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
                uint64_t* bitboard = get_bitboard(board, piece);
                set_bit(bitboard, square);
                file++;
            }
        }
    }
    board->w_occupied = board->w_pawns | board->w_knights | board->w_bishops | board->w_rooks | board->w_queens | board->w_king;
    board->b_occupied = board->b_pawns | board->b_knights | board->b_bishops | board->b_rooks | board->b_queens | board->b_king;
    board->occupied = board->w_occupied | board->b_occupied;

    // Initalize turn
    token = strtok_r(rest, " ", &rest);
    board->turn = (*token == 'w') ? WHITE : BLACK;

    // Initalize castling rights
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

    // Initalize possible en passant square
    token = strtok_r(rest, " ", &rest);
    board->en_passant_square = (*token == '-') ? NULL_SQUARE : parse_square(token);

    // Initalize halfmove clock
    token = strtok_r(rest, " ", &rest);
    board->halfmove_clock = atoi(token);

    // Initalize fullmove number
    token = strtok_r(rest, " ", &rest);
    board->fullmove_number = atoi(token);

    // Initalize zobrist
    _init_zobrist(board);

    // Initalize stack
    Stack* node = malloc(sizeof(Stack));
    node->move = NULL_MOVE;
    node->board = *board;
    node->next = *stack;
    *stack = node;
}


/**
 * Initalizes the zobrist value of the starting position.
 * @param board 
 */
static void _init_zobrist(Board* board) {
    board->zobrist = 0;
    for (int square = A1; square <= H8; square++) {
        char piece = board->mailbox[square];
        if (piece != '-') {
            board->zobrist ^= ZOBRIST_VALUES[64*parse_piece(piece) + square];
        }
    }
    if (board->turn == BLACK) {
        board->zobrist ^= ZOBRIST_VALUES[768];
    }
    if (board->w_kingside_castling_rights) {
        board->zobrist ^= ZOBRIST_VALUES[769];
    }
    if (board->w_queenside_castling_rights) {
        board->zobrist ^= ZOBRIST_VALUES[770];
    }
    if (board->b_kingside_castling_rights) {
        board->zobrist ^= ZOBRIST_VALUES[771];
    }
    if (board->b_queenside_castling_rights) {
        board->zobrist ^= ZOBRIST_VALUES[772];
    }
    if (board->en_passant_square != NULL_SQUARE) {
        board->zobrist ^= ZOBRIST_VALUES[773 + rank_of(board->en_passant_square)];
    }
}


/**
 * Makes the given move. If it is illegal, revert it.
 * @param board 
 * @param stack history of board positions and the moves it took to reach them.
 * @param move
 * @return true if the move was legal
 */
bool legal_push(Board* board, Stack** stack, Move move) {
    int from = move.from;
    int to = move.to;
    int flag = move.flag;

    // Determine if castling is legal
    if (flag == CASTLING) {
        if (is_check(board, board->turn)) return false; // Assert the king is not in check
        if (board->turn == WHITE) {
            if (from != E1) return false; // Assert the king is still alive
            if (to == G1) { // Kingside
                if (!board->w_kingside_castling_rights) return false; // Assert king or rook has not moved
                if (!(board->w_rooks & BB_SQUARES[H1])) return false; // Assert rook is still alive
                if (board->occupied & (BB_SQUARES[F1] | BB_SQUARES[G1])) return false; // Assert there are no pieces between the king and rook
                if (is_attacked(board, BLACK, F1) || is_attacked(board, BLACK, G1)) return false; // Assert the squares the king moves through are not attacked

                push(board, stack, move);
                return true;
            } else if (to == C1) { // Queenside
                if (!board->w_queenside_castling_rights) return false;
                if (!(board->w_rooks & BB_SQUARES[A1])) return false;
                if (board->occupied & (BB_SQUARES[D1] | BB_SQUARES[C1] | BB_SQUARES[B1])) return false;
                if (is_attacked(board, BLACK, D1) || is_attacked(board, BLACK, C1)) return false;

                push(board, stack, move);
                return true;
            }
            return false;
        } else {
            if (from != E8) return false;
            if (to == G8) { // Kingside
                if (!board->b_kingside_castling_rights) return false;
                if (!(board->b_rooks & BB_SQUARES[H8])) return false;
                if (board->occupied & (BB_SQUARES[F8] | BB_SQUARES[G8])) return false;
                if (is_attacked(board, WHITE, F8) || is_attacked(board, WHITE, G8)) return false;

                push(board, stack, move);
                return true;
            } else if (to == C8) { // Queenside
                if (!board->b_queenside_castling_rights) return false;
                if (!(board->b_rooks & BB_SQUARES[A8])) return false;
                if (board->occupied & (BB_SQUARES[D8] | BB_SQUARES[C8] | BB_SQUARES[B8])) return false;
                if (is_attacked(board, WHITE, D8) || is_attacked(board, WHITE, C8)) return false;

                push(board, stack, move);
                return true;
            }
            return false;
        }
    }

    push(board, stack, move);
    if (is_check(board, !board->turn)) {
        pop(board, stack);
        return false;
    }
    return true;
}


/**
 * Makes the given move and updates the stack.
 * @param board
 * @param stack history of board positions and the moves it took to reach them.
 * @param move
 */
void push(Board* board, Stack** stack, Move move) {
    Stack* node = malloc(sizeof(Stack));
    _make_move(board, move);
    node->board = *board;
    node->move = move;
    node->next = *stack;
    *stack = node;
}


/**
 * Unmakes the most recent move and updates the stack.
 * @param board
 * @param stack history of board positions and the moves it took to reach them.
 */
void pop(Board* board, Stack** stack) {
    Stack* temp = *stack;
    *stack = (*stack)->next;
    *board = (*stack)->board;
    free(temp);
}


/**
 * Updates the board with the move.
 * @param board
 * @param move 
 */
static void _make_move(Board* board, Move move) {
    int from = move.from;
    int to = move.to;
    int flag = move.flag;
    bool color = board->turn;

    char attacker = board->mailbox[from];
    char victim = board->mailbox[to];

    if (flag == PASS) {
        board->turn = !color;
        return;
    }

    bool reset_halfmove = false;
    board->en_passant_square = NULL_SQUARE;

    switch (attacker) {
        case 'P':
            clear_bit(&board->w_pawns, from);
            set_bit(&board->w_pawns, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'P';

            reset_halfmove = true;

            if (rank_of(to) - rank_of(from) == 2) board->en_passant_square = to - 8;

            else if (flag == EN_PASSANT) {
                clear_bit(&board->b_pawns, to - 8);
                board->mailbox[to - 8] = '-';
            }

            else if (rank_of(to) == 7) { // Promotions
                switch (flag) {
                    case PROMOTION_QUEEN:
                        clear_bit(&board->w_pawns, to);
                        set_bit(&board->w_queens, to);
                        board->mailbox[to] = 'Q';
                        break;
                    case PROMOTION_ROOK:
                        clear_bit(&board->w_pawns, to);
                        set_bit(&board->w_rooks, to);
                        board->mailbox[to] = 'R';
                        break;
                    case PROMOTION_BISHOP:
                        clear_bit(&board->w_pawns, to);
                        set_bit(&board->w_bishops, to);
                        board->mailbox[to] = 'B';
                        break;
                    case PROMOTION_KNIGHT:
                        clear_bit(&board->w_pawns, to);
                        set_bit(&board->w_knights, to);
                        board->mailbox[to] = 'N';
                        break;
                }
            }

            break;
        case 'N':
            clear_bit(&board->w_knights, from);
            set_bit(&board->w_knights, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'N';
            break;
        case 'B':
            clear_bit(&board->w_bishops, from);
            set_bit(&board->w_bishops, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'B';
            break;
        case 'R':
            clear_bit(&board->w_rooks, from);
            set_bit(&board->w_rooks, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'R';

            if (from == H8) board->w_kingside_castling_rights = false;
            else if (from == A1) board->w_queenside_castling_rights = false;

            break;
        case 'Q':
            clear_bit(&board->w_queens, from);
            set_bit(&board->w_queens, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'Q';
            break;
        case 'K':
            clear_bit(&board->w_king, from);
            set_bit(&board->w_king, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'K';

            if (flag == CASTLING) {
                if (file_of(to) - file_of(from) > 0) { // Kingside
                    clear_bit(&board->w_rooks, H1);
                    set_bit(&board->w_rooks, F1);
                    board->mailbox[H1] = '-';
                    board->mailbox[F1] = 'R';
                } else { // Queenside
                    clear_bit(&board->w_rooks, A1);
                    set_bit(&board->w_rooks, D1);
                    board->mailbox[A1] = '-';
                    board->mailbox[D1] = 'R';
                }
            }

            board->w_kingside_castling_rights = false;
            board->w_queenside_castling_rights = false;

            break;
        case 'p':
            clear_bit(&board->b_pawns, from);
            set_bit(&board->b_pawns, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'p';

            reset_halfmove = true;

            if (rank_of(to) - rank_of(from) == -2) board->en_passant_square = to + 8;

            else if (flag == EN_PASSANT) {
                clear_bit(&board->w_pawns, to + 8);
                board->mailbox[to + 8] = '-';
            }

            else if (rank_of(to) == 0) { // Promotions
                switch (flag) {
                    case PROMOTION_QUEEN:
                        clear_bit(&board->b_pawns, to);
                        set_bit(&board->b_queens, to);
                        board->mailbox[to] = 'q';
                        break;
                    case PROMOTION_ROOK:
                        clear_bit(&board->b_pawns, to);
                        set_bit(&board->b_rooks, to);
                        board->mailbox[to] = 'r';
                        break;
                    case PROMOTION_BISHOP:
                        clear_bit(&board->b_pawns, to);
                        set_bit(&board->b_bishops, to);
                        board->mailbox[to] = 'b';
                        break;
                    case PROMOTION_KNIGHT:
                        clear_bit(&board->b_pawns, to);
                        set_bit(&board->b_knights, to);
                        board->mailbox[to] = 'n';
                        break;
                }
            }

            break;
        case 'n':
            clear_bit(&board->b_knights, from);
            set_bit(&board->b_knights, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'n';
            break;
        case 'b':
            clear_bit(&board->b_bishops, from);
            set_bit(&board->b_bishops, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'b';
            break;
        case 'r':
            clear_bit(&board->b_rooks, from);
            set_bit(&board->b_rooks, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'r';

            if (from == H8) board->b_kingside_castling_rights = false;
            else if (from == A1) board->b_queenside_castling_rights = false;

            break;
        case 'q':
            clear_bit(&board->b_queens, from);
            set_bit(&board->b_queens, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'q';
            break;
        case 'k':
            clear_bit(&board->b_king, from);
            set_bit(&board->b_king, to);
            board->mailbox[from] = '-';
            board->mailbox[to] = 'k';

            if (flag == CASTLING) {
                if (file_of(to) - file_of(from) > 0) { // Kingside
                    clear_bit(&board->b_rooks, H8);
                    set_bit(&board->b_rooks, F8);
                    board->mailbox[H8] = '-';
                    board->mailbox[F8] = 'r';
                } else { // Queenside
                    clear_bit(&board->b_rooks, A8);
                    set_bit(&board->b_rooks, D8);
                    board->mailbox[A8] = '-';
                    board->mailbox[D8] = 'r';
                }
            }

            board->b_kingside_castling_rights = false;
            board->b_queenside_castling_rights = false;

            break;
    }

    if (victim != '-') {
        reset_halfmove = true;
        uint64_t* victim_bb = board->bitboards[parse_piece(victim)];
        clear_bit(victim_bb, to);
    }

    board->w_occupied = board->w_pawns | board->w_knights | board->w_bishops | board->w_rooks | board->w_queens | board->w_king;
    board->b_occupied = board->b_pawns | board->b_knights | board->b_bishops | board->b_rooks | board->b_queens | board->b_king;
    board->occupied = board->w_occupied | board->b_occupied;

    if (reset_halfmove) {
        board->halfmove_clock = 0;
    } else {
        board->halfmove_clock++;
    }

    if (color == BLACK) board->fullmove_number++;

    board->turn = !color;
}


/**
 * @param board 
 * @param color the color of the king.
 * @return true if the side's king is in check.
 */
bool is_check(Board* board, bool color) {
    if (color == WHITE) {
        return is_attacked(board, BLACK, get_lsb(board->w_king));
    } else {
        return is_attacked(board, WHITE, get_lsb(board->b_king));
    }
}


/**
 * @param board 
 * @param color the color of the attackers.
 * @param square 
 * @return true if the square is being attacked by the given side
 */
bool is_attacked(Board* board, bool color, int square) {
    if (color == BLACK) {
        uint64_t square_bb = BB_SQUARES[square];

        if (get_queen_moves(board, WHITE, square) & board->b_queens) return true;
        if (get_rook_moves(board, WHITE, square) & board->b_rooks) return true;
        if (get_bishop_moves(board, WHITE, square) & board->b_bishops) return true;
        if (get_knight_moves(board, WHITE, square) & board->b_knights) return true;
        if ((((square_bb << 9) & ~BB_FILE_A) | ((square_bb << 7) & ~BB_FILE_H)) & board->b_pawns) return true;

        return false;
    } else {
        uint64_t square_bb = BB_SQUARES[square];

        if (get_queen_moves(board, BLACK, square) & board->w_queens) return true;
        if (get_rook_moves(board, BLACK, square) & board->w_rooks) return true;
        if (get_bishop_moves(board, BLACK, square) & board->w_bishops) return true;
        if (get_knight_moves(board, BLACK, square) & board->w_knights) return true;
        if ((((square_bb >> 9) & ~BB_FILE_H) | ((square_bb >> 7) & ~BB_FILE_A)) & board->w_pawns) return true;

        return false;
    }
}


/**
 * @param board 
 * @param piece 
 * @return a pointer to the bitboard of the piece.
 */
uint64_t* get_bitboard(Board* board, char piece) {
    return board->bitboards[parse_piece(piece)];
}
