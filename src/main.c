#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"


int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

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
     * 
     * pos 1 accurate to depth 7
     * pos 2 fails on depth 5
     * pos 3 fails on depth 7
     * pos 4 fails on depth 4
     * pos 5 fails on depth 5
     * pos 6 accurate to depth 5
    **/

    // print_divided_perft(&board, &stack, 1);

    printf("\n%llu\n", board.zobrist);

 
    return 0;
}
