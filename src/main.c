#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

    Board board;
    Stack* stack = malloc(sizeof(Stack));

    init(&board, &stack, fen);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * perft: speedups in move generation for check and checkmate. also not fully accurate until 3-fold rep and 50 move rule detection
     * UCI
    **/
    printf("\n%d", perft(&board, &stack, 2));






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

    // print_mailbox(board.mailbox);

    // printf("Turn: %d\n%d%d%d%d\nEn passant: %d\nHalfmove: %d\nFullmove: %d\n", board.turn,
    //                                                                            board.w_kingside_castling_rights, board.w_queenside_castling_rights, board.b_kingside_castling_rights, board.b_queenside_castling_rights,
    //                                                                            board.en_passant_square,
    //                                                                            board.halfmove_clock,
    //                                                                            board.fullmove_number);

 
    return 0;
}
