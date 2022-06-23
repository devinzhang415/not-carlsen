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
    char* fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

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
     * pos 5 accurate to depth 5
     * pos 6 accurate to depth 5
    **/


    // Move moves[1000];
    // gen_legal_moves(moves, &board, &stack, &rtable, BLACK);
    // for (int i = 0; i < 1000; i++) {
    //     if (moves[i].flag == INVALID) break;
    //     print_move_pre(&board, moves[i]);
    //     printf("\n");
    // }

    print_divided_legal_perft(&board, &stack, &rtable, 6);


    
 
    return 0;
}
