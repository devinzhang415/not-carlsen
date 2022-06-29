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
    char* fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

    init(fen);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * UCI
     * legal move gen optimization
     * - save king square?
    **/

    print_divided_perft(5);


    
 
    return 0;
}
