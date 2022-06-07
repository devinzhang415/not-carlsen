#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "3k4/2r1q3/8/6b1/4n3/8/4p3/3K4 w - - 0 1";

    Board board;
    Stack* stack = malloc(sizeof(Stack));

    init(&board, &stack, fen);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * perft
     * UCI
    **/
    printf("%d\n", is_check(&board, WHITE));




    // Move move = {D5, E6, EN_PASSANT};
    // push(&board, &stack, &move);

    // print_mailbox(board.mailbox);

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
