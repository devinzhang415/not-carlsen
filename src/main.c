#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PP6/KN6 w kq - 0 1";

    Board board;
    Stack* stack = (Stack*) malloc(sizeof(Stack));


    init(&board, &stack, fen);

    Move move1 = {E4, A1, NONE};
    Move move2 = {E8, A1, NONE};

    printf("%d\n", stack->move->from);
    push(&stack, &move1);
    printf("%d\n", stack->move->from);
    push(&stack, &move2);
    printf("%d\n", stack->move->from);

    pop(&stack);
    printf("%d\n", stack->move->from);
 
    return 0;
}
