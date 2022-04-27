#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;

    init_board(&board, "rnbqkbnr/ppp1pp1p/3p4/8/3RpP2/8/PPPPP1PP/1NBQKBNR w Kkq - 0 1");

    init_rook_attacks();
    print_bb(get_rook_attacks(D4, board.occupied));

    return 0;
}
