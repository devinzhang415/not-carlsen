#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnbqkbnr/pp3ppp/8/2ppp3/8/3P4/PPP1PPPP/RNBQKBNR w KQkq - 0 1";

    init_board(&board, fen);

    return 0;
}
