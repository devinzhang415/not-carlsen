#include <stdio.h>
#include "constants.h"
#include "board.h"

int main(void) {
    Board board;

    init_board(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

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
