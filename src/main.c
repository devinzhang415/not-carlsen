#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "rnb1kbnr/4p1pp/8/2pq1pP1/ppPpK3/1P2P3/P2P1P1P/RNBQ1BNR w kq f6 0 1";

    init_board(&board, fen);

    print_bb(get_pawn_moves_all(&board, BLACK));

    return 0;
}
