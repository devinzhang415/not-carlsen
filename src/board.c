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
#include "stack.h"
#include "rtable.h"


extern Board board;
extern Stack* stack;
extern RTable rtable;


/**
 * Initalizes the board and other values.
 * @param fen the FEN string to initalize the board to. Assumed valid.
 */
void init(char* fen) {
    // Initalize misc
    srand(time(NULL));
    init_rtable();
    init_rays();
    init_zobrist_table();
    init_bishop_attacks();
    init_rook_attacks();

    char fen_copy[100];
    strcpy(fen_copy, fen);
    char* rest = fen_copy;

    // Initalize bitboards and mailbox
    char* token = strtok_r(rest, " ", &rest);
    for (int i = A1; i <= H8; i++) {
        board.mailbox[i] = '-';
    }
    board.w_pawns = 0;
    board.w_knights = 0;
    board.w_bishops = 0;
    board.w_rooks = 0;
    board.w_queens = 0;
    board.w_king = 0;
    board.b_pawns = 0;
    board.b_knights = 0;
    board.b_bishops = 0;
    board.b_rooks = 0;
    board.b_queens = 0;
    board.b_king = 0;
    board.bitboards[0] = &board.w_pawns;
    board.bitboards[1] = &board.w_knights;
    board.bitboards[2] = &board.w_bishops;
    board.bitboards[3] = &board.w_rooks;
    board.bitboards[4] = &board.w_queens;
    board.bitboards[5] = &board.w_king;
    board.bitboards[6] = &board.b_pawns;
    board.bitboards[7] = &board.b_knights;
    board.bitboards[8] = &board.b_bishops;
    board.bitboards[9] = &board.b_rooks;
    board.bitboards[10] = &board.b_queens;
    board.bitboards[11] = &board.b_king;
    for (int rank = 7; rank >= 0; rank--) {
        char* fen_board = strtok_r(token, "/", &token);
        int file = 0;
        for (int j = 0; j < strlen(fen_board); j++) {
            if (file >= 8) break;

            char piece = fen_board[j];
            if (isdigit(piece)) {
                file += piece - '0';
            } else {
                int square = 8*rank + file;
                board.mailbox[square] = piece;
                uint64_t* bitboard = get_bitboard(piece);
                set_bit(bitboard, square);
                file++;
            }
        }
    }
    board.w_occupied = board.w_pawns | board.w_knights | board.w_bishops | board.w_rooks | board.w_queens | board.w_king;
    board.b_occupied = board.b_pawns | board.b_knights | board.b_bishops | board.b_rooks | board.b_queens | board.b_king;
    board.occupied = board.w_occupied | board.b_occupied;

    // Initalize king squares
    board.w_king_square = get_lsb(board.w_king);
    board.b_king_square = get_lsb(board.b_king);

    // Initalize turn
    token = strtok_r(rest, " ", &rest);
    board.turn = (*token == 'w') ? WHITE : BLACK;

    // Initalize castling rights
    token = strtok_r(rest, " ", &rest);
    board.w_kingside_castling_rights = false;
    board.w_queenside_castling_rights = false;
    board.b_kingside_castling_rights = false;
    board.b_queenside_castling_rights = false;
    for (int i = 0; i < strlen(token); i++) {
        char piece = token[i];
        switch (piece) {
            case 'K':
                board.w_kingside_castling_rights = true;
                break;
            case 'Q':
                board.w_queenside_castling_rights = true;
                break;
            case 'k':
                board.b_kingside_castling_rights = true;
                break;
            case 'q':
                board.b_queenside_castling_rights = true;
                break;
        }
    }

    // Initalize possible en passant square
    token = strtok_r(rest, " ", &rest);
    board.en_passant_square = (*token == '-') ? INVALID : parse_square_char(token);

    // Initalize halfmove clock
    token = strtok_r(rest, " ", &rest);
    board.halfmove_clock = atoi(token);

    // Initalize fullmove number
    token = strtok_r(rest, " ", &rest);
    board.fullmove_number = atoi(token);

    // Initalize zobrist
    board.zobrist = 0;
    for (int square = A1; square <= H8; square++) {
        char piece = board.mailbox[square];
        if (piece != '-') {
            board.zobrist ^= ZOBRIST_VALUES[64*parse_piece(piece) + square];
        }
    }
    if (board.turn == BLACK) {
        board.zobrist ^= ZOBRIST_VALUES[768];
    }
    if (board.w_kingside_castling_rights) {
        board.zobrist ^= ZOBRIST_VALUES[769];
    }
    if (board.w_queenside_castling_rights) {
        board.zobrist ^= ZOBRIST_VALUES[770];
    }
    if (board.b_kingside_castling_rights) {
        board.zobrist ^= ZOBRIST_VALUES[771];
    }
    if (board.b_queenside_castling_rights) {
        board.zobrist ^= ZOBRIST_VALUES[772];
    }
    if (board.en_passant_square != INVALID) {
        board.zobrist ^= ZOBRIST_VALUES[773 + file_of(board.en_passant_square)];
    }

    // Initalize stack
    init_stack();

    // Add position to threefold history
    rtable_add(board.zobrist);
}


/**
 * Updates the board with the move.
 * @param move 
 */
void _make_move(Move move) {
    int from = move.from;
    int to = move.to;
    int flag = move.flag;
    bool color = board.turn;

    char attacker = board.mailbox[from];
    char victim = board.mailbox[to];

    if (flag == PASS) {
        board.turn = !color;
        board.zobrist ^= ZOBRIST_VALUES[768];
        return;
    }

    bool reset_halfmove = false;

    if (board.en_passant_square != INVALID) {
        board.zobrist ^= ZOBRIST_VALUES[773 + file_of(board.en_passant_square)];
        board.en_passant_square = INVALID;
    }

    uint64_t* attacker_bb = get_bitboard(attacker);
    clear_bit(attacker_bb, from);
    set_bit(attacker_bb, to);
    board.mailbox[from] = '-';
    board.mailbox[to] = attacker;
    board.zobrist ^= ZOBRIST_VALUES[64*parse_piece(attacker) + from];
    board.zobrist ^= ZOBRIST_VALUES[64*parse_piece(attacker) + to];

    switch (attacker) {
        case 'P':
            reset_halfmove = true;

            if (rank_of(to) - rank_of(from) == 2) {
                board.en_passant_square = to - 8;
                board.zobrist ^= ZOBRIST_VALUES[773 + file_of(board.en_passant_square)];
            }

            else if (flag == EN_PASSANT) {
                clear_bit(&board.b_pawns, to - 8);
                board.mailbox[to - 8] = '-';
                board.zobrist ^= ZOBRIST_VALUES[64*6 + (to - 8)];
            }

            else if (rank_of(to) == 7) { // Promotions
                clear_bit(&board.w_pawns, to);
                board.zobrist ^= ZOBRIST_VALUES[64*0 + to];
                switch (flag) {
                    case PROMOTION_QUEEN:
                        set_bit(&board.w_queens, to);
                        board.mailbox[to] = 'Q';
                        board.zobrist ^= ZOBRIST_VALUES[64*4 + to];
                        break;
                    case PROMOTION_ROOK:
                        set_bit(&board.w_rooks, to);
                        board.mailbox[to] = 'R';
                        board.zobrist ^= ZOBRIST_VALUES[64*3 + to];
                        break;
                    case PROMOTION_BISHOP:
                        set_bit(&board.w_bishops, to);
                        board.mailbox[to] = 'B';
                        board.zobrist ^= ZOBRIST_VALUES[64*2 + to];
                        break;
                    case PROMOTION_KNIGHT:
                        set_bit(&board.w_knights, to);
                        board.mailbox[to] = 'N';
                        board.zobrist ^= ZOBRIST_VALUES[64*1 + to];
                        break;
                }
            }

            break;
        case 'R':
            if (from == H1 && board.w_kingside_castling_rights) {
                board.w_kingside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[769];
            } else if (from == A1 && board.w_queenside_castling_rights) {
                board.w_queenside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[770];
            }
            break;
        case 'K':
            board.w_king_square = to;

            if (flag == CASTLING) {
                if (file_of(to) - file_of(from) > 0) { // Kingside
                    clear_bit(&board.w_rooks, H1);
                    set_bit(&board.w_rooks, F1);
                    board.mailbox[H1] = '-';
                    board.mailbox[F1] = 'R';
                    board.zobrist ^= ZOBRIST_VALUES[64*3 + H1];
                    board.zobrist ^= ZOBRIST_VALUES[64*3 + F1];
                } else { // Queenside
                    clear_bit(&board.w_rooks, A1);
                    set_bit(&board.w_rooks, D1);
                    board.mailbox[A1] = '-';
                    board.mailbox[D1] = 'R';
                    board.zobrist ^= ZOBRIST_VALUES[64*3 + A1];
                    board.zobrist ^= ZOBRIST_VALUES[64*3 + D1];
                }
            }

            if (board.w_kingside_castling_rights) {
                board.w_kingside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[769];
            }
            if (board.w_queenside_castling_rights) {
                board.w_queenside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[770];
            }

            break;
        case 'p':
            reset_halfmove = true;

            if (rank_of(to) - rank_of(from) == -2) {
                board.en_passant_square = to + 8;
                board.zobrist ^= ZOBRIST_VALUES[773 + file_of(board.en_passant_square)];
            }

            else if (flag == EN_PASSANT) {
                clear_bit(&board.w_pawns, to + 8);
                board.mailbox[to + 8] = '-';
                board.zobrist ^= ZOBRIST_VALUES[64*0 + (to + 8)];
            }

            else if (rank_of(to) == 0) { // Promotions
                clear_bit(&board.b_pawns, to);
                board.zobrist ^= ZOBRIST_VALUES[64*6 + to];
                switch (flag) {
                    case PROMOTION_QUEEN:
                        set_bit(&board.b_queens, to);
                        board.mailbox[to] = 'q';
                        board.zobrist ^= ZOBRIST_VALUES[64*10 + to];
                        break;
                    case PROMOTION_ROOK:
                        set_bit(&board.b_rooks, to);
                        board.mailbox[to] = 'r';
                        board.zobrist ^= ZOBRIST_VALUES[64*9 + to];
                        break;
                    case PROMOTION_BISHOP:
                        set_bit(&board.b_bishops, to);
                        board.mailbox[to] = 'b';
                        board.zobrist ^= ZOBRIST_VALUES[64*8 + to];
                        break;
                    case PROMOTION_KNIGHT:
                        set_bit(&board.b_knights, to);
                        board.mailbox[to] = 'n';
                        board.zobrist ^= ZOBRIST_VALUES[64*7 + to];
                        break;
                }
            }

            break;
        case 'r':
            if (from == H8 && board.b_kingside_castling_rights) {
                board.b_kingside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[771];
            } else if (from == A8 && board.b_queenside_castling_rights) {
                board.b_queenside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[772];
            }
            break;
        case 'k':
            board.b_king_square = to;

            if (flag == CASTLING) {
                if (file_of(to) - file_of(from) > 0) { // Kingside
                    clear_bit(&board.b_rooks, H8);
                    set_bit(&board.b_rooks, F8);
                    board.mailbox[H8] = '-';
                    board.mailbox[F8] = 'r';
                    board.zobrist ^= ZOBRIST_VALUES[64*9 + H8];
                    board.zobrist ^= ZOBRIST_VALUES[64*9 + F8];
                } else { // Queenside
                    clear_bit(&board.b_rooks, A8);
                    set_bit(&board.b_rooks, D8);
                    board.mailbox[A8] = '-';
                    board.mailbox[D8] = 'r';
                    board.zobrist ^= ZOBRIST_VALUES[64*9 + A8];
                    board.zobrist ^= ZOBRIST_VALUES[64*9 + D8];
                }
            }

            if (board.b_kingside_castling_rights) {
                board.b_kingside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[771];
            }
            if (board.b_queenside_castling_rights) {
                board.b_queenside_castling_rights = false;
                board.zobrist ^= ZOBRIST_VALUES[772];
            }

            break;
    }

    if (victim != '-') {
        reset_halfmove = true;
        uint64_t* victim_bb = get_bitboard(victim);
        clear_bit(victim_bb, to);
        board.zobrist ^= ZOBRIST_VALUES[64*parse_piece(victim) + to];
    }

    board.w_occupied = board.w_pawns | board.w_knights | board.w_bishops | board.w_rooks | board.w_queens | board.w_king;
    board.b_occupied = board.b_pawns | board.b_knights | board.b_bishops | board.b_rooks | board.b_queens | board.b_king;
    board.occupied = board.w_occupied | board.b_occupied;

    if (reset_halfmove) {
        board.halfmove_clock = 0;
    } else {
        board.halfmove_clock++;
    }

    if (color == BLACK) board.fullmove_number++;

    board.turn = !color;
    board.zobrist ^= ZOBRIST_VALUES[768];
}


/**
 * @param color the color of the king.
 * @return true if the side's king is in check.
 */
bool is_check(bool color) {
    if (color == WHITE) {
        return is_attacked(BLACK, get_lsb(board.w_king));
    } else {
        return is_attacked(WHITE, get_lsb(board.b_king));
    }
}


/**
 * @param color the color of the attackers.
 * @param square 
 * @return true if the square is being attacked by the given side
 */
bool is_attacked(bool color, int square) {
    if (color == BLACK) {
        uint64_t square_bb = BB_SQUARES[square];

        if (get_queen_moves(WHITE, square) & board.b_queens) return true;
        if (get_rook_moves(WHITE, square) & board.b_rooks) return true;
        if (get_bishop_moves(WHITE, square) & board.b_bishops) return true;
        if (get_knight_moves(WHITE, square) & board.b_knights) return true;
        if ((((square_bb << 9) & ~BB_FILE_A) | ((square_bb << 7) & ~BB_FILE_H)) & board.b_pawns) return true;

        return false;
    } else {
        uint64_t square_bb = BB_SQUARES[square];

        if (get_queen_moves(BLACK, square) & board.w_queens) return true;
        if (get_rook_moves(BLACK, square) & board.w_rooks) return true;
        if (get_bishop_moves(BLACK, square) & board.w_bishops) return true;
        if (get_knight_moves(BLACK, square) & board.w_knights) return true;
        if ((((square_bb >> 9) & ~BB_FILE_H) | ((square_bb >> 7) & ~BB_FILE_A)) & board.w_pawns) return true;

        return false;
    }
}


/**
 * @return true if the game is ended by:
 * - checkmate // TODO
 * - stalemate // TODO
 * - threefold rep
 * - or 50-move rule
 */
bool is_game_over(void) {
    return (_is_threefold_rep() || _is_fifty_move_rule());
}


/**
 * @return true if the position has occured 3+ times.
 */
static bool _is_threefold_rep(void) {
    return (rtable_get(board.zobrist) >= 3);
}


/**
 * @return true if the position has not had a pawn move or capture in the last 50 full moves.
 */
static bool _is_fifty_move_rule(void) {
    return (board.halfmove_clock >= 50);
}


/**
 * @param piece 
 * @return a pointer to the bitboard of the piece.
 */
uint64_t* get_bitboard(char piece) {
    return board.bitboards[parse_piece(piece)];
}


/**
 * Prints the labeled representation of the mailbox board.
 */
void print_mailbox(void) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file <= 7; file++) {
            printf("%c ", board.mailbox[8*rank + file]);
        }
        printf("\n");
    }
    printf("\n");
}
