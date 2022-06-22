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
    char* fen = "8/8/8/8/k2Pp2Q/8/8/3K4 w - d3 0 1";

    Board board;
    Stack* stack;
    RTable rtable;

    init(&board, &stack, &rtable, fen);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * UCI
     * legal move gen
     * 
     * https://www.chessprogramming.org/Perft_Results
     * pos 1 fails on depth 4
     * pos 2
     * pos 3
     * pos 4
     * pos 5
     * pos 6
    **/


    Move moves[1000];
    gen_legal_moves(moves, &board, BLACK);
    for (int i = 0; i < 1000; i++) {
        if (moves[i].flag == INVALID) break;
        print_move_pre(&board, moves[i]);
        printf("\n");
    }


    // print_divided_legal_perft(&board, &stack, &rtable, 1);

    
 
    return 0;
}
