#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "1r2k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQk - 0 1";

    init_board(&board, fen);

    print_bb(get_king_attacks(&board, E8, BLACK));

    return 0;
}
