#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"
#include "board.h"
#include "movegen.h"
#include "stack.h"
#include "rtable.h"


int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

    Board board;
    Stack* stack;
    RTable rtable;

    init(&board, &stack, &rtable, fen);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * UCI
     * legal move gen
     * - 5.5-6.1 mnps for some reason, need to tackle every area and see what is slow
     * 
     * https://www.chessprogramming.org/Perft_Results
     * pos 1 accurate to depth 7
     * pos 2 accurate to depth 5
     * pos 3 accurate to depth 8
     * pos 4 accurate to depth 6
     * pos 5 fails on depth 5
     * pos 6 accurate to depth 5
    **/


    // Move moves[1000];
    // gen_legal_moves(moves, &board, &stack, &rtable, BLACK);
    // for (int i = 0; i < 1000; i++) {
    //     if (moves[i].flag == INVALID) break;
    //     print_move_pre(&board, moves[i]);
    //     printf("\n");
    // }

    print_divided_legal_perft(&board, &stack, &rtable, 5);
    
 
    return 0;
}
