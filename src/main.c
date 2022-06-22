#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"
#include "stack.h"
#include "rtable.h"


int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

    Board board;
    Stack* stack;
    RTable rtable;

    init(&board, &stack, &rtable, fen);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * perft: speedups in move generation for check and checkmate.
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

    // print_divided_perft(&board, &stack, &rtable, 4);
    rtable.entries = realloc(rtable.entries, rtable.capacity * 2 * sizeof(RTable_Entry));
    for (int i = 0; i < rtable.capacity * 2; i++) {
        printf("%llu, %d, %d\n", rtable.entries[i].key, rtable.entries[i].num, rtable.entries[i].initalized);
    }
 
    return 0;
}
