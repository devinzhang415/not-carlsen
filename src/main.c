#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    ////////////////////////////////////////////////////////////////////////////////
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";

    Board board;
    Stack* stack = malloc(sizeof(Stack));

    init(&board, &stack, DEFAULT_FEN);

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * TODO
     * is_check
     * UCI
    **/

    print_bb(board.occupied);
    Move move = {E2, E4, NONE};
    push(&stack, &move);
    print_bb(board.occupied);
 
    return 0;
}
