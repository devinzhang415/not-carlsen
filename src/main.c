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
    char* fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";

    init(DEFAULT_FEN);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * UCI
    **/

    print_divided_perft(5);


    
 
    return 0;
}
