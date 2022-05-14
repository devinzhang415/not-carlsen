#include <stdio.h>
#include "util.h"
#include "board.h"
#include "movegen.h"

int main(void) {
    Board board;
    char* DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    char* fen = "8/6P1/8/8/P7/8/1P6/8 w - - 0 1";

    init_board(&board, DEFAULT_FEN);

    // while (pieces) {
    //     int square = get_lsb(pieces);
    //     char piece = board->mailbox[square];
    //     pieces ^= 1ULL << square; // Remove the bit of the visited square
    // }

    return 0;
}
