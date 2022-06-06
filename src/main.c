#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";

    Board board;
    Stack* stack = (Stack*) malloc(sizeof(Stack));


    init(&board, &stack, fen);
 
    return 0;
}
