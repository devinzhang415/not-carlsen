#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";

    Board board;
    Stack* stack = malloc(sizeof(Stack));

    init(&board, &stack, DEFAULT_FEN);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * is_check
     * UCI
    **/

    Move e2e4 = {E2, E4, NONE};
    Move e7e5 = {E7, E5, NONE};
    push(&stack, &e2e4);
    push(&stack, &e7e5);

    print_mailbox(board.mailbox);

    printf("Turn: %d\n%d%d%d%d\nEn passant: %d\nHalfmove: %d\nFullmove: %d\n", board.turn,
                                                                               board.w_kingside_castling_rights, board.w_queenside_castling_rights, board.b_kingside_castling_rights, board.b_queenside_castling_rights,
                                                                               board.en_passant_square,
                                                                               board.halfmove_clock,
                                                                               board.fullmove_number);

 
    return 0;
}
