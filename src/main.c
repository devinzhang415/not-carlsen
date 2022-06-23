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
    **/

    print_divided_legal_perft(&board, &stack, &rtable, 4); // 2,103,487


    
 
    return 0;
}
