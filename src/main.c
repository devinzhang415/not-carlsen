#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    // char* fen = "rnQq1k1r/pp2bppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R b KQ - 1 8";

    Board board;
    Stack* stack = malloc(sizeof(Stack));

    init(&board, &stack, fen);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * perft: speedups in move generation for check and checkmate.
     *  also not fully accurate until 3-fold rep and 50 move rule detection
     * UCI
     * legal move gen
    **/

    printf("\n%llu", perft(&board, &stack, 2));

    // print_bb(board.b_occupied);
    // Move move = {D7, C8, PROMOTION_QUEEN};
    // legal_push(&board, &stack, move);
    // print_bb(board.b_occupied);

    // Move moves[1000];
    // gen_pseudolegal_moves(moves, &board, BLACK);
    // for (int i = 0; i < 1000; i++) {
    //     if (moves[i].flag == INVALID) break;
    //     if (legal_push(&board, &stack, moves[i])) {
    //         print_move_post(&board, moves[i]);
    //         printf(" ");
    //     }
    //     pop(&board, &stack);
    // }

 
    return 0;
}
