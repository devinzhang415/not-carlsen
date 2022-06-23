#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"
#include "board.h"
#include "movegen.h"
#include "stack.h"
#include "rtable.h"


Board board;
Stack* stack;
RTable rtable;


int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";

    init(DEFAULT_FEN);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * UCI
     * legal move gen optimization
    **/

    print_divided_legal_perft(6);
    // Move moves[MAX_MOVE_NUM];
    // int n = gen_legal_moves(moves, WHITE);
    // printf("%d\n", n);


    
 
    return 0;
}
