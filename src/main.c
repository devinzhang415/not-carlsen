#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "8/8/8/1bb1bb2/3b4/1p6/2K4r/8 w - - 0 1";

    init_board(&board, fen);

    print_bb(_get_checkmask(&board));
    // print_bb(_get_ray_between(C2, F5));

    // while (attackers) {
    //     int square = get_lsb(attackers);
    //     char piece = board->mailbox[square];
    //     attackers ^= 1ULL << square; // Remove the bit of the visited square

    return 0;
}
